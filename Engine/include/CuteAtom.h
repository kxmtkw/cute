
#ifndef CUTE_ATOM_H
#define CUTE_ATOM_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	ctAtomType_NoneType  = 0x0,
	ctAtomType_Int       = 0x1,
	ctAtomType_UInt      = 0x2,
	ctAtomType_Float     = 0x3,
	ctAtomType_Bool      = 0x4,
	ctAtomType_Container = 0x5
} ctAtomType;


typedef uint8_t ctAtomTypeSize;


typedef union {
	uint64_t raw;
	int64_t  as_int;
	uint64_t as_uint;
	uint64_t as_bool;
	double   as_float;
} ctAtom;


typedef struct {
	ctAtomType type;
	ctAtom     atom;
} ctTypedAtom;


static inline ctTypedAtom
ct_atom_pack(ctAtom atom, ctAtomTypeSize type) {
    return (ctTypedAtom){type, atom};
}

static const char* ct_atom_stringforms[] = {
	"none",
	"int",
	"uint",
	"float",
	"bool",
	"container"
};

#endif // CUTE_ATOM_H