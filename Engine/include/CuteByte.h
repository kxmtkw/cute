
#ifndef CUTE_BYTE_H
#define CUTE_BYTE_H

#include <stdint.h>

typedef enum {

	instrHalt    = 0x00,
	instrNull    = 0x01,

	instrOut     = 0x10,

	instrMov     = 0x20,
	instrLoadI   = 0x21,
	instrLoadU   = 0x22,
	instrLoadF   = 0x23,

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

	instrCmpI    = 0x60,
	instrCmpF    = 0x61,
	instrCheckEq = 0x62,
	instrCheckLt = 0x63,
	instrCheckLe = 0x64,
	instrCheckGt = 0x65,
	instrCheckGe = 0x66,

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