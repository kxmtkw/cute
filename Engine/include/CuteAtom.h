
#ifndef CUTE_ATOM_H
#define CUTE_ATOM_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	CtAtomType_Int = 0,
	CtAtomType_UInt = 1,
	CtAtomType_Float = 2,
	CtAtomType_Container = 3
} CtAtomType;

typedef uint64_t CtAtom;

#define CTATOM_MASK(atom, type) \
(atom & ~((int64_t)0x3 << 62)) | ((int64_t)type << 62)

#define CTATOM_ISINT(atom)   (((atom) >> 62) == CtAtomType_Int)
#define CTATOM_ISUINT(atom)  (((atom) >> 62) == CtAtomType_UInt)
#define CTATOM_ISFLOAT(atom) (((atom) >> 62) == CtAtomType_Float)

#endif // CUTE_ATOM_H