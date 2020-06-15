/*
 * cavallino_utility.c
 *
 *  Created on: Jun 12, 2020
 *      Author: 15103
 */

//==============================================================================
// Include files
#include "cavallino_utility.h"

//==============================================================================
// Constants


//==============================================================================
// Types


//==============================================================================
// Static functions


//==============================================================================
// Global functions
int utility_logError(int error, const char *errorMsg) {
	if (error < 0) {
		syslog(LOG_ERR, errorMsg);
		printf("error: %d; \nmessage: %s\n", error, errorMsg);
	}
	return error;
}

