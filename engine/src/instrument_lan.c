/*
 * instrument_lan.c
 *
 *  Created on: Jun 16, 2020
 *      Author: 15103
 */

//==============================================================================
// Include files
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bits/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cavallino_utility.h"
#include "instrument_lan.h"

//==============================================================================
// Constants
#define VISA_BUFFER_SIZE	64

#define VISA_READ_NO_DATA_RETURNING		-64001
#define VISA_READ_DATA_NOT_DOUBLE		-64002
#define VISA_QUERY_ERROR				-64003
#define VISA_EMPTY_RESOURCE_NAME		-64004
#define VISA_DEVICE_HAS_NOT_INITIALIZED -64005
#define VISA_LIBRARY_API_ERROR 			-64006
#define VISA_SOCKET_CONNECTION_FAILED	-64007
#define VISA_SET_NODELAY_OPTION_FAILURE -64008

#define SOCKETS_PORT				5025
#define SOCKETS_BUFFER_SIZE			1024
#define SOCKETS_TIMEOUT				10

//==============================================================================
// Types


//==============================================================================
// Static global variables
LAN_Device 	device_list 	= NULL;


//==============================================================================
// Static functions
int LAN_InitializeDevice(LAN_Device *pDevice, const char *instrAddress, char errorMsg[]);
int LAN_SendCmd(int conn, const char *cmd, char errorMsg[]);
int LAN_WaitForData(int conn, unsigned int waitMS, char errorMsg[]);
int LAN_ReadData(int conn, unsigned char *buffer, int *dataLength, char errorMsg[]);

//==============================================================================
// Global functions
int LAN_FindDevice(LAN_Device *pDevice, const char *instrAddress,
		char createIfNotExist, char *foundDevice, char errorMsg[]) {
	int 		error 	= 0,
				found	= 0;
	ErrMsg		errMsg	= {0};
	LAN_Device	tmp 	= NULL,
				end		= NULL;

	// check instrument address
	if (!(instrAddress && strlen(instrAddress)>8)) {
		sprintf(errMsg, "invalid resource name(%s)", instrAddress);
		errChk(VISA_EMPTY_RESOURCE_NAME);
	}

	// search device_list for instrument address
	if (device_list) {
		for (tmp = device_list; tmp->nextDevice; tmp = tmp->nextDevice) {
			if (strcmp(tmp->instrName, instrAddress) == 0) {
				break;
			}
			if (strcmp(tmp->instrName, instrAddress) == 0) {
				*pDevice = tmp;
				found	 = 1;
			}
		}
	}

	// create if not exists
	if (!found) {
		if (createIfNotExist>0) {
			errChk(LAN_InitializeDevice(&end, instrAddress, errMsg));
			*pDevice = end;
		}

		if (device_list)
			tmp->nextDevice = end;
		else
			device_list = end;
	} else {
		sprintf(errMsg, "instrument (%s) has not been initialized", instrAddress);
		errChk(VISA_DEVICE_HAS_NOT_INITIALIZED);
	}
Error:
	if (foundDevice)	*foundDevice = found;
	reportError();
	return error;
}


int LAN_InitializeDevice(LAN_Device *pDevice, const char *instrAddress, char errorMsg[]) {
	int					error 		= 0,
						StateNODELAY= 1;
	ErrMsg				errMsg		= {0};
	LAN_Device			tmp 		= NULL;
	struct sockaddr_in	myAddress 	= { .sin_family = PF_INET,
										.sin_port = htons(SOCKETS_PORT),
										.sin_addr.s_addr = inet_addr(instrAddress)};

	nullChk(tmp = (LAN_Device) malloc(sizeof(*tmp)));
	memset(tmp, 0, sizeof(*tmp));
	nullChk(tmp->instrName = malloc(sizeof(instrAddress)+1));
	strcpy(tmp->instrName, instrAddress);
	tmp->nextDevice = NULL;

	errChk(tmp->instrHandle = socket(PF_INET, SOCK_STREAM, 0));
	error = connect(tmp->instrHandle, (struct sockaddr *) &myAddress, sizeof(myAddress));
	if (error < 0) {
		sprintf(errMsg, "can not establish connection to %s", instrAddress);
		errChk(VISA_SOCKET_CONNECTION_FAILED);
	}
	error = setsockopt(tmp->instrHandle, IPPROTO_TCP, TCP_NODELAY,
			(void *)&StateNODELAY, sizeof(StateNODELAY));
	if (error < 0) {
		sprintf(errMsg, "can not set NODELAY option(%i)...", errno);
		errChk(VISA_SET_NODELAY_OPTION_FAILURE);
	}
	errChk(LAN_SendCmd(tmp->instrHandle, "*CLS;*RST\n", errMsg));
	*pDevice = tmp;
Error:
	reportError();
	return error;
}


int LAN_IdentifyDevice(LAN_Device device, char *deviceName,
		unsigned int waitMS, char *identified, char errorMsg[]) {
	int 			error 						= 0,
					returnLength				= 0;
	ErrMsg			errMsg						= {0};
	unsigned char	buffer[VISA_BUFFER_SIZE]	= {0};
	const char		*cmd						= "*IDN?\n";

	*identified = 0;
	errChk(LAN_SendCmd(device->instrHandle, cmd, errMsg));
	errChk(LAN_WaitForData(device->instrHandle, waitMS, errMsg));
	errChk(LAN_ReadData(device->instrHandle, buffer, &returnLength, errMsg));
	if (returnLength>0) {
		memcpy(deviceName, buffer, returnLength);
		deviceName[returnLength] = 0;
	}
Error:
	reportError();
	return error;
}


int LAN_Close(LAN_Device *pDevice, char *found, char errorMsg[]) {
	LAN_Device	tmp = device_list;

	*found = 0;
	if (device_list && *pDevice) {
		if (strcmp(device_list->instrName, (*pDevice)->instrName) == 0) {
			*found		= 1;
			device_list = tmp->nextDevice;
		} else {
			for (tmp = device_list; tmp->nextDevice; tmp = tmp->nextDevice) {
				if (strcmp((*pDevice)->instrName, tmp->nextDevice->instrName) == 0) {
					*found = 1;
					tmp->nextDevice = (*pDevice)->nextDevice;
					break;
				}
			}
		}
	}

	if (*found) {
		if ((*pDevice)->instrHandle)
			close((*pDevice)->instrHandle);
		if ((*pDevice)->instrName)
			free((*pDevice)->instrName);
		free(*pDevice);
		*pDevice = NULL;
	}
	return 0;
}


int LAN_QueryNumeric(LAN_Device device, const char *cmd, int waitMS,
		unsigned char buffer[], double *data, char errorMsg[]) {
	int				error					= 0,
					dataLength				= 0;
	ErrMsg			errMsg					= {0};
	unsigned char	buf[VISA_BUFFER_SIZE] 	= {0};

	errChk(LAN_SendCmd(device->instrHandle, cmd, errMsg));
	sleep(waitMS*1000);
	errChk(LAN_ReadData(device->instrHandle, buf, &dataLength, errMsg));
	if (dataLength == 0) {
		strcpy(errMsg, "there is no data returning");
		errChk(VISA_READ_NO_DATA_RETURNING);
	}
	// convert to double
	sscanf((char *)buf, "%lf", data);
	if (buffer) {
		memcpy(buffer, buf, dataLength);
		buffer[dataLength] = 0;
	}
Error:
	reportError();
	return error;
}


int LAN_Destroy() {
	LAN_Device 	f = NULL;

	while (device_list) {
		f = device_list;
		device_list = device_list->nextDevice;

		if (f->instrHandle)
			close(f->instrHandle);
		if (f->instrName)
			free(f->instrName);
		free(f);
	}
	return 0;
}


int LAN_SendCmd(int conn, const char *cmd, char errorMsg[]) {
	int 		error		= 0;

	error = send(conn, cmd, strlen(cmd), 0);
	if (error < 0)
		sprintf(errorMsg, "LAN send command error (%s)", cmd);
	return error;
}



int LAN_WaitForData(int conn, unsigned int waitMS, char errorMsg[]) {
	int				error	= 0;
	ErrMsg			errMsg	= {0};
	fd_set 			MyFDSet;
	struct 	timeval timeout	= {.tv_sec = waitMS/1000, .tv_usec = 1000*(waitMS%1000)};

	// Wait for data to become available
	FD_ZERO(&MyFDSet); // Initialize fd_set structure
	FD_SET(conn, &MyFDSet); // Add socket to "watch list"
	error = select(conn+1, &MyFDSet, NULL, NULL, &timeout); // Wait for change

	// 0 = timeout, 1 = socket status has changed
	if (error == -1)
		sprintf(errMsg, "Error: Problem with select (%i)...\n",errno);
	else if (error == 0) {
		strcpy(errMsg, "There is no data available");
		error = -1;
	}
	reportError();
	return error;
}


int LAN_ReadData(int conn, unsigned char *buffer, int *dataLength, char errorMsg[]) {
	int 		error		= 0;

	error = recv(conn, (void *)buffer, VISA_BUFFER_SIZE, 0);
	if (error < 0) {
		strcpy(errorMsg, __FUNCTION__);
	} else {
		if (dataLength)	*dataLength = error;
	}
	return error;
}
