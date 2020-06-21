/*
 * cavallino_client.h
 *
 *  Created on: Jun 15, 2020
 *      Author: Chenny Wang
 */

#ifndef CAVALLINO_CLIENT_H_
#define CAVALLINO_CLIENT_H_

//==============================================================================
// Include files
#ifdef _CVI_
#include "windows.h"
#define WaitSec(timeoutSec) 	Sleep(1000*(timeoutSec))
#else
#include <unistd.h>
#define WaitSec(timeoutSec)		sleep(timeoutSec)
#endif
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

//==============================================================================
// Constants
#define OUT_OF_MEMORY 					-10001


#ifndef errChk
#define errChk(fCall) if (error = (fCall), error < 0) \
{goto Error;} else
#endif

#ifndef nullChk
#define nullChk(fCall) if ((fCall) == 0) \
{error = OUT_OF_MEMORY; goto Error;} else
#endif

#define reportError()	if (error < 0)	sprintf(errorMsg, "%s->%s", __FUNCTION__, errMsg);

#define ERRMSG_SIZE     512
#define ENGINE_IP				"192.168.1.84"
#define CAVALLINO_REQUESTER		"tcp://%s:14538"
#define CAVALLINO_SUBSCRIBER	"tcp://%s:14537"
#define CAVALLINO_FIFODATA		"tcp://%s:14535"

//==============================================================================
// Types
typedef char 			ErrMsg[ERRMSG_SIZE];

typedef enum {
	cmd_start		= 0,
	cmd_shutdown	= 1,
	cmd_set_rate	= 2,
	cmd_get_rate	= 3,
	cmd_dma_fifo	= 4,
} CMD_Mode;


//==============================================================================
// Static variables


//==============================================================================
// Static functions


//==============================================================================
// Global functions
int client_initialize(const char *targetIp, char errorMsg[]);
int client_close(void);
int client_request(CMD_Mode mode, unsigned int *data,
		int *returnData, char errorMsg[]);
int client_cmdDmaFifo(unsigned char cmd, unsigned char data,
		unsigned short additional, int *returnData, char errorMsg[]);


#endif /* CAVALLINO_CLIENT_H_ */
