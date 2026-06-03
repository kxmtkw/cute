
#ifndef ENGINE_ERROR_H
#define ENGINE_ERROR_H

#include <stdint.h>
#include <stdio.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"


typedef enum {
	ctErrorCode_None               = 0x00,
	ctErrorCode_IllegalInstruction = 0x01,
	ctErrorCode_EngineFailure      = 0x02,
	ctErrorCode_OutOfBounds        = 0x03,

	ctErrorCode_TypeError          = 0x10,
	ctErrorCode_ValueError         = 0x11,
	ctErrorCode_ZeroDivision       = 0x12,
	ctErrorCode_IndexError         = 0x13,
	ctErrorCode_OverflowError      = 0x14,
	ctErrorCode_UnderflowError     = 0x15,

	ctErrorCode_RecursionDepth     = 0x20,

	ctErrorCode_FileNotFound       = 0x30,

} ctErrorCode;


static const char* ct_error_stringforms[] = {
	[ctErrorCode_None]               = "NoError",
	[ctErrorCode_IllegalInstruction] = "IllegalInstruction",
	[ctErrorCode_EngineFailure]      = "EngineFailure",
	[ctErrorCode_OutOfBounds]        = "OutOfBoundsError",

	[ctErrorCode_TypeError]          = "TypeError",
	[ctErrorCode_ValueError]         = "ValueError",
	[ctErrorCode_ZeroDivision]       = "ZeroDivisionError",
	[ctErrorCode_IndexError]         = "IndexError",
	[ctErrorCode_OverflowError]      = "OverflowError",
	[ctErrorCode_UnderflowError]     = "UnderflowError",

	[ctErrorCode_RecursionDepth]     = "RecursionError",

	[ctErrorCode_FileNotFound]       = "FileNotFoundError",
};


typedef struct {
	ctErrorCode code;
	char*       msg;
} ctError;


static inline ctError
ct_error_make(ctErrorCode code, char* message) {
	ctError err;
	err.code = code;
	err.msg = message;
	return err;
}

static inline void
ct_error_print(ctError err) {
	printf("[%s] %s\n", ct_error_stringforms[err.code], err.msg);
}

#endif // ENGINE_ERROR_H