/*
 * engine.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Chenny Wang
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#ifdef __cplusplus
    extern "C" {
#endif
//==============================================================================
// Include files
#include "cavallino_utility.h"
#include "cavallino_fpga.h"
#include "visa_utility.h"

//==============================================================================
// Constants
#define CAVALLINO_RESPONDER		"tcp://*:14538"
#define CAVALLINO_PUBLISHER		"tcp://*:14537"
#define CAVALLINO_FIFODATA		"tcp://*:14535"

#define BUFFER_SIZE				10
#define INSTRUMENT_IP_ADDRESS	"192.168.1.90"

//==============================================================================
// Types
typedef enum {
	cmd_start		= 0,
	cmd_shutdown	= 1,
	cmd_set_rate	= 2,
	cmd_get_rate	= 3,
	cmd_dma_fifo	= 4,
	cmd_query_instr = 5,
} CMD_Mode;

typedef struct {
	CMD_Mode		mode;
	unsigned int	data;
	unsigned char	raw[BUFFER_SIZE];
} MSG_CMD;

//==============================================================================
// Static functions


//==============================================================================
// Global functions





#ifdef __cplusplus
    }
#endif

#endif /* ENGINE_H_ */
