
#ifndef CUTE_INSTR_H
#define CUTE_INSTR_H

#include <stdint.h>

typedef enum {

	instrHalt    = 0x00,
	instrNull    = 0x01,

	instrOut     = 0x10,
	instrTypeof  = 0x11,
	
	instrMov     = 0x20,
	instrLoad    = 0x21,
	instrStore   = 0x22,
	instrLoadG   = 0x23,
	instrStoreG  = 0x24,
	instrSetI    = 0x25,
	instrSetU    = 0x26,
	instrSetF    = 0x27,
	instrSetB    = 0x28,
	instrSetC    = 0x29,
	instrSetN    = 0x2A,

	instrAddI    = 0x30,
	instrSubI    = 0x31,
	instrMulI    = 0x32,
	instrDivI    = 0x33,
	instrModI    = 0x34,
	instrNegI    = 0x35,
	instrAbsI    = 0x36,

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
	instrAbsF    = 0x55,	

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
	instrJmpIfNot    = 0xA2,

	instrJmpAbs          = 0xA3,
	instrJmpAbsIf        = 0xA4,
	instrJmpAbsIfNot     = 0xA5,

	instrCall        = 0xB0,
	instrReturn      = 0xB1,
	instrModCall     = 0xB2,

	instrConNew     = 0xC1,
	instrConDel     = 0xC2,
	instrConGet     = 0xC3,
	instrConSet     = 0xC4,
	instrConClone   = 0xC5,
	instrConResize  = 0xC6,
	instrConLen     = 0xC7

} ctInstruction;

typedef uint8_t ctInstructionSize;


static const uint32_t ctMagicId = 0x12345678; 

typedef struct {
	uint32_t magic_id;
	uint32_t procedure_count;
	uint32_t instruction_count;
	uint32_t procedure_table_offset;
	uint32_t instruction_pool_offset; 
} ctImageHeader;


typedef struct {
	uint32_t id;
	uint32_t bytecode_index;
	uint32_t locals_count;
} ctImageProcedure;


typedef struct {
	ctImageHeader      header;
	ctImageProcedure*  procedure_table;
	ctInstructionSize* instruction_pool;
} ctImage;


typedef enum {
	ctImageCode_Success,
	ctImageCode_FileNotFound,
	ctImageCode_ReadWriteFailure,
	ctImageCode_InvalidImage
} ctImageCode;


// Write an already intialized image to a file
ctImageCode 
ct_image_write(ctImage *img, const char *filepath);

// Set  an image from a file
ctImageCode 
ct_image_read(ctImage *img, const char *filepath);

// Free the image's resources.
void 
ct_image_free(ctImage *img);

// Print the image for debugging
void 
ct_image_print(const ctImage* img);

#endif // CUTE_INSTR_H
