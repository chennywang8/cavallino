/*
 * cavallino_zmq_res.c
 *
 *  Created on: Jun 12, 2020
 *      Author: 15103
 */

//==============================================================================
// Include files
#include "cavallino_utility.h"
#include "cavallino_zmq_res.h"

//==============================================================================
// Constants


//==============================================================================
// Types


//==============================================================================
// Static variables
void 	*responder	= NULL;

//==============================================================================
// Static functions


//==============================================================================
// Global functions
int zmq_res_initialize(void *context, char errorMsg[]) {
	int 	error 	= 0;
	ErrMsg	errMsg	= {0};

	responder 	= zmq_socket(context, ZMQ_REP);

Error:
	reportError();
	return error;
}

int zmq_res_fetch(CMD_Mode *mode, char errorMsg[]) {
	int 	error 	= 0;
	ErrMsg	errMsg	= {0};

Error:
	reportError();
	return error;
}

int zmq_res_close(char errorMsg[]) {
	int 	error 	= 0;
	ErrMsg	errMsg	= {0};

Error:
	reportError();
	return error;
}
