
#ifndef CUTE_BYTE_H
#define CUTE_BYTE_H

#include <stdint.h>

typedef enum {

	instrHalt    = 0x00,
	instrNull    = 0x01,

	instrOut     = 0x10,
	instrTypeOf  = 0x11,

	instrMov     = 0x20,
	instrLoadI   = 0x21,
	instrLoadU   = 0x22,
	instrLoadF   = 0x23,
	instrLoadB   = 0x24,

	instrAddI    = 0x30,
	instrSubI    = 0x31,
	instrMulI    = 0x32,
	instrDivI    = 0x33,
	instrModI    = 0x34,
	instrNegI    = 0x35,

	instrAddU    = 0x40,
	instrSubU    = 0x41,
	instrMulU    = 0x42,
	instrDivU    = 0x43,
	instrModU    = 0x44,

	instrAddF    = 0x50,
	instrSubF    = 0x51,
	instrMulF    = 0x52,
	instrDivF    = 0x53,
	instrNegF    = 0x54,

	instrLogicAnd    = 0x60,
	instrLogicOr     = 0x61,
	instrLogicNot    = 0x62,

	instrBitAnd      = 0x70,
	instrBitOr       = 0x71,
	instrBitNot      = 0x73,
	instrBitXor      = 0x74,
	instrBitShl      = 0x75,
	instrBitShr      = 0x76, 

	instrCmpI        = 0x80,
	instrCmpU        = 0x81,
	instrCmpF        = 0x82,

	instrEq          = 0x90,
	instrNotEq       = 0x91,
	instrLess        = 0x92,
	instrLessEq      = 0x93,
	instrGreater     = 0x94,
	instrGreaterEq   = 0x95,

	instrJmp         = 0xA0,
	instrJmpIf       = 0xA1,
	instrJmpIfNot    = 0xA2

} CtInstruction;

typedef uint8_t CtInstructionSize;

// Image Structure


typedef struct {
	uint32_t magic_id;
	uint64_t procedure_count;
	uint64_t instruction_count;
} CtImageHeader;


typedef struct {
	uint64_t id;
	uint64_t bytecode_index;
	uint32_t locals_size;
} CtImageProcedure;


typedef struct {
	CtImageHeader header;
	CtImageProcedure* procedure_table;
	CtInstructionSize* instruction_pool;
} CtImage;


typedef enum {
	CtImageCode_Success,
	CtImageCode_FileNotFound,
	CtImageCode_ReadWriteFailure,
	CtImageCode_InvalidImage
} CtImageCode;


void 
CtImage_init(CtImage *img);

void 
CtImage_del(CtImage *img);

CtImageCode 
CtImage_write(CtImage *img, const char *filepath);

CtImageCode 
CtImage_read(CtImage *img, const char *filepath);

void CtImage_print(const CtImage* img);

static const uint32_t CtMagicId = 0x12345678; 

#endif // CUTE_BYTE_H