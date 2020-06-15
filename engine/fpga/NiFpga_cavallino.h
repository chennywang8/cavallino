/*
 * Generated with the FPGA Interface C API Generator 19.0
 * for NI-RIO 19.0 or later.
 */
#ifndef __NiFpga_cavallino_h__
#define __NiFpga_cavallino_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 190
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_cavallino_Bitfile;
 */
#define NiFpga_cavallino_Bitfile "NiFpga_cavallino.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_cavallino_Signature = "2BAFC20D8C4AC52372F85B208F841DEC";

#if NiFpga_Cpp
extern "C"
{
#endif

typedef enum
{
   NiFpga_cavallino_ControlBool_StartDMA = 0x18006,
} NiFpga_cavallino_ControlBool;

typedef enum
{
   NiFpga_cavallino_IndicatorArrayU16_raw = 0x18000,
} NiFpga_cavallino_IndicatorArrayU16;

typedef enum
{
   NiFpga_cavallino_IndicatorArrayU16Size_raw = 4,
} NiFpga_cavallino_IndicatorArrayU16Size;

typedef enum
{
   NiFpga_cavallino_TargetToHostFifoU16_FIFO_DATA = 0,
} NiFpga_cavallino_TargetToHostFifoU16;

typedef enum
{
   NiFpga_cavallino_HostToTargetFifoU32_FIFO_CMD = 1,
} NiFpga_cavallino_HostToTargetFifoU32;


#if NiFpga_Cpp
}
#endif

#endif
