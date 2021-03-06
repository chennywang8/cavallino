/*
 * cavallino_fpga.h
 *
 *  Created on: Jun 12, 2020
 *      Author: 15103
 */

#ifndef CAVALLINO_FPGA_H_
#define CAVALLINO_FPGA_H_

#ifdef __cplusplus
    extern "C" {
#endif
//==============================================================================
// Include files
#include <pthread.h>
#include "cavallino_utility.h"
#include "NiFpga_cavallino.h"

//==============================================================================
// Constants


//==============================================================================
// Types
typedef struct {
	char 			terminate;
	char			started;
	int				pollPeriodMs;
	NiFpga_Session	session;
	pthread_t		threadId;
	void*			publisher;
	int 			error;
	ErrMsg			errMsg;
}FPGA_Setup;

//==============================================================================
// Static functions
static const char *FIFO_Cmd[] = {
	"NOPE",
	"set_register",
	"flush_fifo_data",
	"TBD",
	"set_led"
};

//==============================================================================
// Global functions
int fpga_initialize(FPGA_Setup *fpga, char errorMsg[]);
int fpga_close(FPGA_Setup *fpga);

#ifdef __cplusplus
    }
#endif

#endif /* CAVALLINO_FPGA_H_ */
