/*
 * instrument_lan.h
 *
 *  Created on: Jun 16, 2020
 *      Author: 15103
 */

#ifndef INSTRUMENT_LAN_H_
#define INSTRUMENT_LAN_H_

//==============================================================================
// Include files


//==============================================================================
// Constants


//==============================================================================
// Types
typedef struct device *LAN_Device;
struct device {
	int			instrHandle;
	char		*instrName;
	LAN_Device	nextDevice;
};

//==============================================================================
// Static functions


//==============================================================================
// Global functions
int LAN_FindDevice(LAN_Device *pDevice, const char *instrAddress,
		char createIfNotExist, char *foundDevice, char errorMsg[]);
int LAN_QueryNumeric(LAN_Device device, const char *cmd, int waitMS,
		unsigned char buffer[], double *data, char errorMsg[]);
int LAN_IdentifyDevice(LAN_Device device, char *deviceName,
		unsigned int waitMS, char *identified, char errorMsg[]);
int LAN_Close(LAN_Device *pDevice, char *found, char errorMsg[]);
int LAN_Destroy();

#endif /* INSTRUMENT_LAN_H_ */
