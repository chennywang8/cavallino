/*
 ============================================================================
 Name        : rt_server.c
 Author      : Chenny Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
//==============================================================================
// Include files
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <time.h>

//==============================================================================
// Constants
#define SOCK_PATH 					"/tmp/sim_socket"

#ifndef errChk
#define errChk(fCall) if (error = (fCall), error < 0) \
{goto Error;} else
#endif

#define reportError()	if (error < 0)	sprintf(errorMsg, "%s->%s", __FUNCTION__, errMsg);

#define ERRMSG_SIZE     512

//==============================================================================
// Types
typedef char 			ErrMsg[ERRMSG_SIZE];

//==============================================================================
// Static functions


//==============================================================================
// Global functions
int main(int argc, char *argv[]) {
	int 				error 		= 0,
						sfd 		= 0,
						cfd 		= 0,
						i 			= 0;
	ErrMsg  			errMsg 		= {0};
	struct sockaddr_un	addr;
	double 				data[4] 	= {0};		

	// initialize stream socket
	errChk(sfd = socket(AF_UNIX, SOCK_STREAM, 0));
	memset(&addr, 0, sizeof(addr));
	addr.sun_family	= AF_UNIX;
	strcpy(addr.sun_path, SOCK_PATH);
	// remove socket file if exists
	if (access(SOCK_PATH, F_OK) != -1) {
		errChk(remove(SOCK_PATH));
	}
	errChk(bind(sfd, (struct sockaddr *) &addr, sizeof(addr)));
	errChk(listen(sfd, 1));
	puts("sim server started...");

	while(1) {
		errChk(cfd = accept(sfd, NULL, NULL));
		puts("sim server accept conn...");

		for (i = 0; i < 10; i++) {
			data[0] = (double) i;
			data[1] = 2*i+0.5;
			data[2] = i*i;
			data[3] = (int) time(NULL);
			usleep(200*1000);
			if(write(cfd, data, sizeof(data))<0) 	break;
		}
	}
Error:
	if (error<0)	puts(errMsg);
	return error;
}