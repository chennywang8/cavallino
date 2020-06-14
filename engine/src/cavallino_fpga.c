/*
 * cavallino_fpga.c
 *
 *  Created on: Jun 12, 2020
 *      Author: Chenny Wang
 */

//==============================================================================
// Include files
#include "cavallino_fpga.h"

//==============================================================================
// Constants


//==============================================================================
// Types


//==============================================================================
// Static functions
void* fpga_publishData(void *fpgaSetting);

//==============================================================================
// Global functions
int fpga_initialize(FPGA_Setup *fpga, char errorMsg[]) {
	int 		error 		= 0;
	ErrMsg		errMsg		= {0};
	const char 	bitFile[]  	= "/home/admin/"NiFpga_cavallino_Bitfile;

	// check bitFile before starting everything
	if (access(bitFile, F_OK) <0) {
		sprintf(errMsg, "fpga file %s does not exist", bitFile);
		errChk(FILE_NOT_FOUND);
	}
	error = NiFpga_Initialize();
	if (error < 0) {
		strcpy(errMsg, "fpga initialization failed");
		errChk(FPGA_INITIALIZATION_FAILURE);
	}
	// get reference for fpga target
	errChk(NiFpga_Open(bitFile, NiFpga_cavallino_Signature,
			FPGA_TARGET_CAVALLINO, 0, &fpga->session));
	// start fpga target
	errChk(NiFpga_Run(fpga->session, 0));
	// --------------------------------------------
	// launch new thread to poll data from FPGA to shared memory
	error = pthread_create(&fpga->threadId, NULL,
			(void *) fpga_publishData, fpga);
	if (error) {
		strcpy(errMsg, "can not create thread for data collection");
		errChk(PTHREAD_CREATION_FAILURE);
	}

	puts("start fpga server...");
Error:
	reportError();
	return error;
}


// ---------- CLOSE FPGA REF ----------
int fpga_close(FPGA_Setup *fpga) {
	if (fpga) {
		if (fpga->threadId) {
			pthread_join(fpga->threadId, NULL);
		}
		NiFpga_Close(fpga->session, 0);
	}
	NiFpga_Finalize();
	return 0;
}


// ----------- PUBLISH DATA ----------
void* fpga_publishData(void *fpgaSetup) {
	int 		error 	= 0,
				bufSize = sizeof(uint16_t)*FPGA_DATA_SIZE;
	uint16_t	buffer[FPGA_DATA_SIZE]	= {0};
	FPGA_Setup 	*fpga	= (FPGA_Setup*) fpgaSetup;

	sleep(1);
	while(!fpga->terminate) {
		usleep(1000*fpga->pollPeriodMs);/*
		errChk(NiFpga_ReadArrayU16(fpga->session,
				NiFpga_cavallino_IndicatorArrayU16_raw,
				buffer, bufSize));*/
		errChk(zmq_send(fpga->publisher, buffer, bufSize, 0));

	}
Error:
	fpga->error = error;
	return (void*) &fpga->error;
}
