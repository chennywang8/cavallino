//==============================================================================
//
// Title:       visa_utility.c
// Purpose:     A short description of the implementation.
//
// Created on:  1/30/2017 at 6:33:51 PM by GalaTech.
// Copyright:   GalaTech, Inc.. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include "visa_utility.h"


//==============================================================================
// Constants
 
//==============================================================================
// Types

//==============================================================================
// Static global variables
VISA_Device device_list = NULL; 

//==============================================================================
// Static functions
int VISA_InitializeDevice(VISA_Device *pDevice, const char *visa, char errorMsg[]);

//==============================================================================
// Global variables

//==============================================================================
// Global functions
int VISA_FindDevice(VISA_Device *pDevice, const char *visa, char createIfNotExist, char *foundDevice, char errorMsg[])
{
	int 			error 	= 0,
					found 	= 0;
	ErrMsg			errMsg 	= {0};
	VISA_Device		tmp 	= NULL,
					end 	= NULL;
	
	// check visa resource name
	if (visa==NULL || strlen(visa)==0) {
		strcpy(errMsg, "VISA resource name can't be empty");
		errChk(VISA_EMPTY_RESOURCE_NAME);
	}
	
	// if device_list exists, search for reource name
	if (device_list) {
		// loop exluding the last element
		for (tmp = device_list; tmp->next_device; tmp = tmp->next_device) {
			if (strcmp(visa, tmp->visa_name)==0)
				break;
		}
		
		//
		if (strcmp(visa, tmp->visa_name)==0) {
			*pDevice = tmp;
			found = 1;
		}
	}
	
	if (!found) {
		if (createIfNotExist) {
			// create new device using visa name
			errChk(VISA_InitializeDevice(&end, visa, errMsg));
			*pDevice = end;
		
			// update device_list
			if (device_list) {	// new device, add it to the end of device_list
				tmp->next_device = end;
			} else {
				// device_list has not been initialized at all 
				device_list = end;
			}
		} else {
			sprintf(errMsg, "device[%s] has not been initialized", visa);
			errChk(VISA_DEVICE_HAS_NOT_INITIALIZED);
		}
	}
Error:
	if (foundDevice)	*foundDevice = (short) found;
	reportError();
	return error;	
}


// --------------- QUERY NUMERIC DATA -------------------
int VISA_QueryNumeric(VISA_Device device, const char *cmd, int waitMS,
		unsigned char buffer[], double *data, char errorMsg[])
{
	int 			error 					= 0;
	ViUInt32		numCount 				= 0;
	ErrMsg			errMsg 					= {0};
	unsigned char	buf[VISA_BUFFER_SIZE] 	= {0};

	//*data = NotANumber();
	visaErrChk(device->instrHandle, viWrite(device->instrHandle,
			(unsigned char *) cmd, (ViUInt32) strlen(cmd), VI_NULL));
	sleep(waitMS);
	visaErrChk(device->instrHandle, viRead(device->instrHandle,
			buf, (ViUInt32) VISA_BUFFER_SIZE, &numCount));
	if (numCount == 0) {
		strcpy(errMsg, "There is no data returning");
		errChk(VISA_READ_NO_DATA_RETURNING);
	}
	// convert to double
	sscanf((char *)buf, "%lf", data);
	if (buffer)	{
		memcpy(buffer, buf, numCount);
		buffer[numCount] = 0;
	}
Error:
	reportError();
	return error;	
}


// -------------- CLOSE SINGLE DEVICE ----------------
void VISA_Close(VISA_Device *pDevice, char *found, char errorMsg[])
{
	VISA_Device	tmp = device_list;
	
	*found = 0;
	if (device_list && *pDevice) {
		// remove device from the top if on top or only one device in the list
		if (strcmp(device_list->visa_name, (*pDevice)->visa_name)==0) {
			*found = 1;
			device_list = tmp->next_device;
		} else {
			for (tmp = device_list; tmp->next_device; tmp = tmp->next_device) {
				if (strcmp((*pDevice)->visa_name, tmp->next_device->visa_name)==0) {
					*found = 1;
					tmp->next_device = (*pDevice)->next_device;
					break;
				} 
			}
		}
		
		if (*found) {
			if ((*pDevice)->instrHandle)
				viClose((*pDevice)->instrHandle);
			free(*pDevice);
			*pDevice = NULL;
		}
	}
}

// ------------ DESTROY VISA LIST -----------------
void VISA_Destroy()
{
	VISA_Device	f = NULL;
	
	while(device_list) {
		f = device_list;
		device_list = device_list->next_device;
		
		if (f->instrHandle)
			viClose(f->instrHandle);
		free(f); 
	}
}


// ------------ INITIALIZE VISA DEVICE --------------
int VISA_InitializeDevice(VISA_Device *pDevice, const char *visa, char errorMsg[])
{
	int 			error 	= 0;
	ErrMsg			errMsg 	= {0};
	VISA_Device		tmp 	= NULL;
	
	nullChk(tmp = (VISA_Device) malloc(sizeof(*tmp)));
	strcpy(tmp->visa_name, visa);
	tmp->next_device = NULL;
	tmp->instrHandle = 0;
	tmp->mngHandle = 0;

	// create visa session
	errChk(viOpenDefaultRM(&tmp->mngHandle));
	errChk(viOpen(tmp->mngHandle, tmp->visa_name, VI_NULL, VI_NULL, &tmp->instrHandle));
	errChk(viClear(tmp->instrHandle));
	*pDevice = tmp;
Error:
	if (error<0 && tmp) {
		viStatusDesc(tmp->instrHandle, error, errMsg);
	}
	reportError();
	return error;
}


// ------------- READ VISA IDENTITY ---------------
int VISA_IdentifyDevice(VISA_Device device, char *deviceName,
		unsigned int waitMS, char *identified, char errorMsg[])
{
	int 			error 					= 0;
	ViUInt32		numCount 				= 0;
	ErrMsg			errMsg 					= {0};
	char			buf[VISA_BUFFER_SIZE]	= {0},
					cmd[] 					= "*IDN?";
	
	*identified = 0;
	visaErrChk(device->instrHandle, viWrite(device->instrHandle,
			(unsigned char *) cmd, (ViUInt32) strlen(cmd), VI_NULL));
	sleep(waitMS);
	visaErrChk(device->instrHandle, viRead(device->instrHandle,
			(unsigned char *) buf, (ViUInt32) VISA_BUFFER_SIZE, &numCount));
	
	if (numCount>0) {
		*identified = 1;
		strncpy(deviceName, buf, numCount);
		deviceName[numCount] = 0;
	}
Error:
	reportError();
	return error;
}
