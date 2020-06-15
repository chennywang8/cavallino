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
#define FPGA_TARGET 	"RIO0" // normally this won't be changed

//==============================================================================
// Types


//==============================================================================
// Static functions
void* fpga_publishData(void *fpgaSetup);
void* fpga_publishDmaFifoData(void *fpgaSetup);

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
	// always invoke this function at the very beginning
	error = NiFpga_Initialize();
	if (error < 0) {
		strcpy(errMsg, "fpga initialization failed");
		errChk(FPGA_INITIALIZATION_FAILURE);
	}
	// get reference for FPGA target
	errChk(NiFpga_Open(bitFile, NiFpga_cavallino_Signature,
			FPGA_TARGET, 0, &fpga->session));
	// start fpga target to access FPGA IO
	errChk(NiFpga_Run(fpga->session, 0));

	// --------------------------------------------
	// launch new thread to poll data from FPGA to shared memory
	error = pthread_create(&fpga->threadId, NULL,
			(void *) fpga_publishData, fpga);
	if (error) {
		strcpy(errMsg, "can not create thread for data collection");
		errChk(PTHREAD_CREATION_FAILURE);
	}

	fpga->started = 1;
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
	fpga->started = 0;
	return 0;
}


// ----------- PUBLISH DATA ----------
void* fpga_publishData(void *fpgaSetup) {
	int 		error 	= 0;
	FPGA_Setup 	*fpga	= (FPGA_Setup*) fpgaSetup;
	uint16_t	buffer[NiFpga_cavallino_IndicatorArrayU16Size_raw]	= {0};

	sleep(1);
	while(!fpga->terminate) {
		usleep(1000*fpga->pollPeriodMs);
		// ---------------------------------
		// use array length instead array size, be careful!!!
		errChk(NiFpga_ReadArrayU16(fpga->session,
				NiFpga_cavallino_IndicatorArrayU16_raw,
				buffer, NiFpga_cavallino_IndicatorArrayU16Size_raw));
		// ---------------------------------
		// use array size
		errChk(zmq_send(fpga->publisher, buffer, sizeof(buffer), 0));
		printf("%d, %d, %d, %d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
	}
Error:
	fpga->error = error;
	return (void*) &fpga->error;
}


// ------------- PUBLISH DMA FIFO DATA ---------
void* fpga_publishDmaFifoData(void *fpgaSetup) {
	int 		error 				= 0;
	size_t		elementsRemaining 	= 0;
	FPGA_Setup 	*fpga				= (FPGA_Setup*) fpgaSetup;
	uint16_t	buffer[4] 			= {0};

	while(!fpga->terminate) {
		errChk(NiFpga_ReadFifoU16(fpga->session,
				NiFpga_cavallino_TargetToHostFifoU16_FIFO_DATA,
				buffer, 4, -1, &elementsRemaining));
		errChk(zmq_send(fpga->publisher, buffer, sizeof(buffer), 0));
	}
Error:
	fpga->error = error;
	return (void*) &fpga->error;
}
