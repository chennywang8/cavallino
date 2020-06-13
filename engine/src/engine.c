/*
 ============================================================================
 Name        : engine.c
 Author      : Chenny Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
//==============================================================================
// Include files
#include "engine.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct {
	void *con;
	void *socCmd;
	void *socData;
} MSG_ZMQ;


//==============================================================================
// Static functions
int engine_initialize(MSG_ZMQ *zmq, char errorMsg[]);
int engine_close(MSG_ZMQ *zmq);
int engine_listenRequest(void *socket, MSG_CMD *cmd, char errorMsg[]);

//==============================================================================
// Global functions
int main (void)
{
	int			error 		= 0;
	ErrMsg		errMsg		= {0};
	MSG_CMD		cmd			= {};
	MSG_ZMQ		zmq			= {};
	FPGA_Setup	fpga		= {};

	errChk(engine_initialize(&zmq, errMsg));
	fpga.publisher 			= zmq.socData;
	fpga.pollPeriodMs 		= 1000;

	while(1) {
		errChk(engine_listenRequest(zmq.socCmd, &cmd, errMsg));
		switch(cmd.mode) {
		case cmd_start: // start fpga
			errChk(fpga_initialize(&fpga, errMsg));
			break;
		case cmd_shutdown:
			fpga.terminate = 1;
			goto Error;
			break;
		case cmd_set_rate:
			if (cmd.data>0) {
				error = fpga.pollPeriodMs = cmd.data;
			} else {

			}
			break;
		case cmd_get_rate:
			error = fpga.pollPeriodMs;
			break;
		case cmd_dma_fifo:
			error = cmd.data;
			break;
		default:

			break;
		}
		errChk(zmq_send(zmq.socCmd, (void *)&error, sizeof(error), 0));
	}
Error:
	fpga_close(&fpga);
	engine_close(&zmq);
	return error;
}


// ------- engine_initialize ----------
// open zmq sockets for
// 1. responder for listening incoming cmd
// 2. publisher for most recent data update
int engine_initialize(MSG_ZMQ *zmq, char errorMsg[]) {
	int		error 	= 0;
	ErrMsg	errMsg	= {0};

	nullChk(zmq->con = zmq_ctx_new());
	// open socket for responder
	nullChk(zmq->socCmd = zmq_socket(zmq->con, ZMQ_REP));
	errChk(zmq_bind(zmq->socCmd, CAVALLINO_RESPONDER));
	// open socket for publisher
	nullChk(zmq->socData = zmq_socket(zmq->con, ZMQ_PUB));
	errChk(zmq_bind(zmq->socData, CAVALLINO_PUBLISHER));

	puts("cavallino engine start...\n");
Error:
	reportError();
	return error;
}


// -------- engine_close ---------
// close all sockets
int engine_close(MSG_ZMQ *zmq) {
	if (zmq->socData)
		zmq_close(zmq->socData);
	if (zmq->socCmd)
		zmq_close(zmq->socCmd);
	if (zmq->con)
		zmq_ctx_shutdown(zmq->con);
	return 0;
}


// ------- engine_listenRequest -----------
// get request and parse incoming message
int engine_listenRequest(void *socket, MSG_CMD *cmd, char errorMsg[]){
	int		error 	= 0;
	ErrMsg	errMsg	= {0};

	cmd->data = -1;
	errChk(zmq_recv(socket, cmd->raw, BUFFER_SIZE, 0));
	cmd->mode = (CMD_Mode) cmd->raw[0];
	if (error>1) {
		cmd->data = U32(cmd->raw[1], cmd->raw[2], cmd->raw[3], cmd->raw[4]);
	}
Error:
	reportError();
	return error;
}
