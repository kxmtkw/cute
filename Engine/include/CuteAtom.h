
#ifndef CUTE_ATOM_H
#define CUTE_ATOM_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	CtAtomType_NoneType = 0x0,
	CtAtomType_Int = 0x1,
	CtAtomType_UInt = 0x2,
	CtAtomType_Float = 0x3,
	CtAtomType_Container = 0x4
} CtAtomType;

typedef uint8_t CtAtomTypeSize;

typedef union {
	uint64_t raw;
	int64_t as_int;
	uint64_t as_uint;
	double as_float;
} CtAtom;

typedef struct {
	CtAtomType type;
	CtAtom atom;
} CtTypedAtom;

#endif // CUTE_ATOM_H