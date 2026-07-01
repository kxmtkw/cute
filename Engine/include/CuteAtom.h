
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

struct _ctContainer;


typedef union {
	uint64_t             raw;
	int64_t              as_int;
	uint64_t             as_uint;
	uint64_t             as_bool;
	double               as_float;
	struct _ctContainer* as_container;
} ctAtom;


struct _ctContainer {
	uint32_t         bucket_id;
	uint32_t         bucket_index;
	uint32_t         ref_count;
	uint32_t         size;
	uint32_t         sub_containers;
	ctAtom*          atoms;
	ctAtomTypeSize*  types;
};

typedef struct _ctContainer ctContainer;


typedef struct {
	ctAtomTypeSize type;
	ctAtom         atom;
} ctTypedAtom;


static inline ctTypedAtom
ct_atom_pack(ctAtom atom, ctAtomTypeSize type) {
    return (ctTypedAtom){type, atom};
}



static const char* ct_atom_stringforms[] = {
	[ctAtomType_NoneType]  = "none",
	[ctAtomType_Int]       = "int",
	[ctAtomType_UInt]      = "uint",
	[ctAtomType_Float]     = "float",
	[ctAtomType_Bool]      = "bool",
	[ctAtomType_Container] = "container"
};

#endif // CUTE_ATOM_H