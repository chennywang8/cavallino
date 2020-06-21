/*
 * cavallino_client.c
 *
 *  Created on: Jun 15, 2020
 *      Author: 15103
 */

//==============================================================================
// Include files
#include "cavallino_client.h"


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
// Static variables
static	MSG_ZMQ	*zmq 	= NULL;

//==============================================================================
// Static functions
int client_packUInt(unsigned char *dataArray, unsigned int dataUnit);
int client_waitSec(unsigned int timeoutSec);

//==============================================================================
// Global functions
int main(void) {
	int 		error 	= 0,
				data	= 0;
	ErrMsg		errMsg	= {0};
	unsigned int rate	= 200;

	puts("start client...");
	errChk(client_initialize(ENGINE_IP, errMsg));
	errChk(client_request(cmd_start, NULL, &data, errMsg));
	client_waitSec(5);
	errChk(client_request(cmd_set_rate, &rate, &data, errMsg));
	client_waitSec(5);
	errChk(client_cmdDmaFifo(1, 3, 333, &data, errMsg));
	client_waitSec(5);
Error:
	errChk(client_request(cmd_shutdown, NULL, &data, errMsg));
	client_close();
	puts("client stopped...");
	return error;
}


// ------------ INITIALIZE CLIENT ----------------
int client_initialize(const char *targetIp, char errorMsg[]) {
	int			error	= 0;
	ErrMsg		errMsg	= {0};
	char		buf[256]= {0};

	nullChk(targetIp);
	client_close();
	nullChk(zmq = malloc(sizeof(MSG_ZMQ)));
	memset(zmq, 0, sizeof(MSG_ZMQ));
	nullChk(zmq->con = zmq_ctx_new());
	nullChk(zmq->socCmd = zmq_socket(zmq->con, ZMQ_REQ));
	sprintf(buf, CAVALLINO_REQUESTER, targetIp);
	errChk(zmq_connect(zmq->socCmd, buf));

	nullChk(zmq->socData = zmq_socket(zmq->con, ZMQ_SUB));
	sprintf(buf, CAVALLINO_SUBSCRIBER, targetIp);
	errChk(zmq_connect(zmq->socData, buf));
Error:
	reportError();
	return error;
}


// -------------- CLOSE CLIENT -------------------
int client_close(void) {
	if (zmq) {
		if (zmq->socCmd)
			zmq_close(zmq->socCmd);
		if (zmq->socData)
			zmq_close(zmq->socData);
		if (zmq->socDma)
			zmq_close(zmq->socDma);
		if (zmq->con)
			zmq_ctx_destroy(zmq->con);
		free(zmq);
	}
	zmq = NULL;
	return 0;
}


// ------------ SEND REQUEST ----------------
int client_request(CMD_Mode mode, unsigned int *data,
		int *returnData, char errorMsg[]) {
	int 			error 		= 0,
					response[2] = {0};
	ErrMsg			errMsg		= {0};
	unsigned char	cmd[10]		= {0};

	nullChk(zmq);
	cmd[0] = mode;
	if (data) 		client_packUInt(cmd+1, *data);

	errChk(zmq_send(zmq->socCmd, cmd, 10, 0));
	errChk(zmq_recv(zmq->socCmd, response, sizeof(response), 0));
	errChk(response[0]);
Error:
	reportError();
	return error;
}


// ------------ SEND DMA_FIFO_CMD ---------------
int client_cmdDmaFifo(unsigned char cmd, unsigned char data,
		unsigned short additional, int *returnData, char errorMsg[]) {
	int				error		= 0;
	uint32_t		fifoCmd		= 0;
	ErrMsg			errMsg		= {0};

	fifoCmd = ((uint32_t) cmd)<<24 | ((uint32_t) data)<<16 | ((uint32_t)additional & 0x0000FFFF);
	errChk(client_request(cmd_dma_fifo, &fifoCmd, returnData, errMsg));
Error:
	reportError();
	return error;
}

// ================== PRIVATE ====================
// ------------- PACK SMALL ENDIAN ---------------
int client_packUInt(unsigned char *dataArray, unsigned int dataUnit) {
	dataArray[0] = (dataUnit & 0x000000FFU);
	dataArray[1] = (dataUnit & 0x0000FF00U)>>8;
	dataArray[2] = (dataUnit & 0x00FF0000U)>>16;
	dataArray[3] = (dataUnit & 0xFF000000U)>>24;
	return 0;
}


int client_waitSec(unsigned int timeoutSec) {
#ifdef _CVI_
	Sleep(1000*timeoutSec); // has to convert to mlisecond
#else
	sleep(timeoutSec);
#endif
	return 0;
}