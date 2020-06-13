/*
 ============================================================================
 Name        : engine.c
 Author      : Chenny Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "engine.h"


int main (void)
{
	int			error 		= 0;
	ErrMsg		errMsg		= {0};
	CMD_Mode	mode		= 0;
	void 		*context	= NULL;

	nullChk(context = zmq_ctx_new());
	errChk(zmq_res_initialize(context, errMsg));
	while(1) {
		errChk(zmq_res_fetch(&mode, errMsg));
		switch(mode) {
		case cmd_start: // start fpga

			break;
		case cmd_shutdown:
			goto Error;
			break;
		case cmd_set_rate:

			break;
		case cmd_pause:

			break;
		case cmd_dma_fifo:

			break;
		default:

			break;
		}
	}
Error:
	zmq_res_close(errMsg);
	if (context)	zmq_ctx_shutdown(context);
	return error;
}
