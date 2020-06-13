
//==============================================================================
// Include files
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <time.h>

//==============================================================================
// Constants
#define OUT_OF_MEMORY 	-10001

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

//==============================================================================
// Types
typedef char 			ErrMsg[ERRMSG_SIZE];


//==============================================================================
// Static functions


//==============================================================================
// Global functions
