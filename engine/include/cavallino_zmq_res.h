/*
 * cavallino_zmq_res.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Chenny Wang
 */

#ifndef CAVALLINO_ZMQ_RES_H_
#define CAVALLINO_ZMQ_RES_H_
//==============================================================================
// Include files
#include <zmq.h>
#include "cavallino_utility.h"

//==============================================================================
// Constants


//==============================================================================
// Types
typedef enum {
	cmd_start		= 0,
	cmd_shutdown	= 1,
	cmd_set_rate	= 2,
	cmd_pause		= 3,
	cmd_dma_fifo	= 4,
} CMD_Mode;

//==============================================================================
// Static variables


//==============================================================================
// Static functions


//==============================================================================
// Global functions

int zmq_res_initialize(void *context, char errorMsg[]);

int zmq_res_fetch(CMD_Mode *mode, char errorMsg[]);

int zmq_res_close(char errorMsg[]);

#endif /* CAVALLINO_ZMQ_RES_H_ */
