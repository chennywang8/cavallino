//==============================================================================
//
// Title:       visa_utility.h
// Purpose:     A short description of the interface.
//
// Created on:  4/30/2017 at 6:33:51 PM by GalaTech.
// Copyright:   GalaTech, Inc.. All Rights Reserved.
//
//==============================================================================

#ifndef __visa_utility_H__
#define __visa_utility_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include <visa.h>
#include "cavallino_utility.h"

//==============================================================================
// Constants
#define VISA_BUFFER_SIZE	64
		
#define VISA_READ_NO_DATA_RETURNING		-10001
#define VISA_READ_DATA_NOT_DOUBLE		-10002
#define VISA_QUERY_ERROR				-10003
#define VISA_EMPTY_RESOURCE_NAME		-10004
#define VISA_DEVICE_HAS_NOT_INITIALIZED -10005
#define VISA_LIBRARY_API_ERROR 			-10006

		
#define visaErrChk(viSession, fCall)\
\
if ((error = (fCall)) != 0) {\
	viStatusDesc(viSession, error, errMsg);\
	error = VISA_LIBRARY_API_ERROR;\
	goto Error;\
} else
		
		
//==============================================================================
// Types
typedef struct device *VISA_Device;
struct device
{
	ViSession 		mngHandle;
	char			visa_name[100];
	ViSession		instrHandle;
	double			data;
	VISA_Device		next_device;
};

//==============================================================================
// External variables

//==============================================================================
// Global functions
int VISA_FindDevice(VISA_Device *pDevice, const char *visa,
		char createIfNotExist, char *foundDevice, char errorMsg[]);
int VISA_QueryNumeric(VISA_Device device, const char *cmd, int waitMS,
		unsigned char buffer[], double *data, char errorMsg[]);
int VISA_IdentifyDevice(VISA_Device device, char *deviceName,
		unsigned int waitMS, char *identified, char errorMsg[]);
void VISA_Close(VISA_Device *pDevice, char *found, char errorMsg[]);
void VISA_Destroy();

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __visa_utility_H__ */
