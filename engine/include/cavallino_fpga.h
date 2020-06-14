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
#define FPGA_TARGET_CAVALLINO 	"RIO0"
#define FPGA_DATA_SIZE			NiFpga_cavallino_IndicatorArrayU16Size_raw

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


//==============================================================================
// Global functions
int fpga_initialize(FPGA_Setup *fpga, char errorMsg[]);
int fpga_close(FPGA_Setup *fpga);

#ifdef __cplusplus
    }
#endif

#endif /* CAVALLINO_FPGA_H_ */
