
/*
 * cavallino_utility.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Chenny Wang
 */

#ifndef CAVALLINO_UTILITY_H_
#define CAVALLINO_UTILITY_H_

#ifdef __cplusplus
    extern "C" {
#endif
//==============================================================================
// Include files
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <zmq.h>
#include <time.h>

//==============================================================================
// Constants
#define OUT_OF_MEMORY 					-10001
#define FPGA_INITIALIZATION_FAILURE		-20001
#define MUX_END_OF_INDEX 				-20002
#define SHARED_MEMORY_ALLO_FAILURE		-30001
#define PTHREAD_CREATION_FAILURE 		-30002
#define SOKCET_INITIALIZATION_FAILURE	-50001
#define LISTEN_TO_CONN_FAILURE 			-50002
#define LIN_EMPTY_SCHEDULE_NAME			-50003
#define	FILE_NOT_FOUND 					-80001
#define NULL_MEASUREMENT_NAME	 		-80002
#define INVALID_MEASUREMENT_NAME 		-80003

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
#define U16(h,l)    	(((uint16_t)(h)<<8)|((uint16_t)(l)&0x00FFU))      // pack bytes into Uint16 word
#define U32(a, b, c, d)    \
    (((uint32_t)(a)<<24) | ((uint32_t)((b) & 0xFFU)<<16) | \
     ((uint32_t)((c) & 0xFFU)<<8) | ((uint32_t)((d) & 0xFFU)<<0))

//==============================================================================
// Types
typedef char 			ErrMsg[ERRMSG_SIZE];


//==============================================================================
// Static functions


//==============================================================================
// Global functions
int utility_logError(int error, const char *errorMsg);

#ifdef __cplusplus
    }
#endif

#endif /* CAVALLINO_UTILITY_H_ */
