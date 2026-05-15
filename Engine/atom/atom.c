#include "CuteAtom.h"

bool
CtAtom_isInt(CtAtom* atom, int64_t* as_int) {

	CtAtomType type = *atom >> 62;

	if (type != 0) {
		return false;
	}

	*as_int = *atom << 2;
	return true;
}

bool
CtAtom_isUInt(CtAtom* atom, uint64_t* as_uint) {

	CtAtomType type = *atom >> 62;

	if (type != 1) {
		return false;
	}

	*as_uint = *atom << 2;
	return true;
}

bool
CtAtom_isFloat(CtAtom* atom, double* as_float) {

	CtAtomType type = *atom >> 62;

	if (type != 2) {
		return false;
	}

	*as_float = *atom << 2;
	return true;
}