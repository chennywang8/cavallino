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
	void *socDma;
} MSG_ZMQ;


//==============================================================================
// Static functions
int engine_initialize(MSG_ZMQ *zmq, char errorMsg[]);
int engine_close(MSG_ZMQ *zmq);
int engine_listenRequest(void *socket, MSG_CMD *cmd, char errorMsg[]);
int engine_getPowerSupplyName(const char *ipAddress, char errorMsg[]);

//==============================================================================
// Global functions
int main (void)
{
	int			error 		= 0,
				rep[2]		= {0};
	ErrMsg		errMsg		= {0};
	MSG_CMD		cmd			= {};
	MSG_ZMQ		zmq			= {};
	FPGA_Setup	fpga		= {};

	errChk(engine_initialize(&zmq, errMsg));
	fpga.publisher 			= zmq.socData;
	fpga.pollPeriodMs 		= 1000;

	while(1) {
		errChk(engine_listenRequest(zmq.socCmd, &cmd, errMsg));
		rep[1] = time(NULL);	// get current timestamp
		switch(cmd.mode) {
		case cmd_start: // start fpga
			if (fpga.started) {
				strcpy(errMsg, "Server has been started already");
				rep[0] = -1;
			} else {
				errChk(fpga_initialize(&fpga, errMsg));
				rep[0] = error;
			}
			break;
		case cmd_shutdown:
			strcpy(errMsg, "Server shutting down...");
			rep[0] = fpga.terminate = 1;
			break;
		case cmd_set_rate:
			sprintf(errMsg, "Set polling rate %dms\n", cmd.data);
			if (cmd.data>0) {
				rep[0] = fpga.pollPeriodMs = cmd.data;
			} else {
				rep[0] = -999;
			}
			break;
		case cmd_get_rate:
			sprintf(errMsg, "Get polling rate %dms\n", fpga.pollPeriodMs);
			rep[0] = fpga.pollPeriodMs;
			break;
		case cmd_dma_fifo:
			sprintf(errMsg, "Write dma-fifo: %u; cmd: %s(%d)-%d-%d\n",
					cmd.data, FIFO_Cmd[cmd.raw[4]], cmd.raw[4],
					cmd.raw[3], U16(cmd.raw[2], cmd.raw[1]));
			rep[0] = (int) cmd.raw[4];
			errChk(NiFpga_WriteFifoU32(fpga.session,
					NiFpga_cavallino_HostToTargetFifoU32_FIFO_CMD,
					&cmd.data, 1, -1, NULL));
			break;
		case cmd_query_instr:
			sprintf(errMsg, "Query device name for %s", INSTRUMENT_IP_ADDRESS);
			errChk(engine_getPowerSupplyName(INSTRUMENT_IP_ADDRESS, errMsg));
			rep[0] = (int) cmd_query_instr;
			break;
		default:
			rep[0] = -1;
			sprintf(errMsg, "Invalid request(%d)", cmd.mode);
			break;
		}
		if (*errMsg)		syslog(LOG_DEBUG, errMsg);
		errChk(zmq_send(zmq.socCmd, (void *)rep, sizeof(rep), 0));
		if (fpga.terminate)	break;
	}
Error:
	fpga_close(&fpga);
	utility_logError(error, errMsg);
	utility_logError(fpga.error, fpga.errMsg);
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

	// start syslog
	openlog("cavallino", LOG_PERROR|LOG_PID, LOG_USER);
	// new zmq context
	nullChk(zmq->con = zmq_ctx_new());
	// open socket for responder
	nullChk(zmq->socCmd = zmq_socket(zmq->con, ZMQ_REP));
	errChk(zmq_bind(zmq->socCmd, CAVALLINO_RESPONDER));
	// open socket for publisher
	nullChk(zmq->socData = zmq_socket(zmq->con, ZMQ_PUB));
	errChk(zmq_bind(zmq->socData, CAVALLINO_PUBLISHER));
	// open socket for publisher - dma fifo data
	nullChk(zmq->socDma = zmq_socket(zmq->con, ZMQ_PUB));
	errChk(zmq_bind(zmq->socDma, CAVALLINO_FIFODATA));
	syslog(LOG_INFO, "Cavallino engine started.");
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
	if (zmq->socDma)
		zmq_close(zmq->socDma);
	if (zmq->con)
		zmq_ctx_shutdown(zmq->con);
	syslog(LOG_INFO, "Cavallino engine stopped.");
	// stop syslog
	closelog();
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
		// small-endian order
		cmd->data = U32(cmd->raw[4], cmd->raw[3], cmd->raw[2], cmd->raw[1]);
	}
Error:
	reportError();
	return error;
}


// -------- query ps device name -----------
// get power supply name and model
int engine_getPowerSupplyName(const char *ipAddress, char errorMsg[]) {
	int			error 			= 0;
	ErrMsg		errMsg			= {0};
	LAN_Device 	dev				= NULL;
	char 		status			= 0,
				buffer[128] 	= {0};

	errChk(LAN_FindDevice(&dev, ipAddress, 1, &status, errMsg));
	errChk(LAN_IdentifyDevice(dev, buffer, 3000, &status, errMsg));
	syslog(LOG_INFO, buffer);
Error:
	LAN_Close(&dev, &status, errMsg);
	reportError();
	return error;
}
