
#ifndef CUTE_ENGINE_H
#define CUTE_ENGINE_H

#include <stdint.h>
#include "CuteAtom.h"
#include "CuteByte.h"


typedef struct {
	uint64_t procedure_id;
	uint32_t locals_count;
	CtAtom* local_atoms;
} CtCallFrame;


typedef struct {
	CtAtom atoms[32];
	CtAtomTypeSize types[32];
} CtRegisterFile;

typedef struct {
	CtImage* image;
	CtCallFrame call_frame;
	CtRegisterFile registers;
	uint64_t	ip;
} CtContext;

#define CT_VALIDREGISTER(r, regfile) assert((r) < sizeof(regfile.atoms) / sizeof(regfile.atoms[0]))


typedef struct {
	CtContext ctx;
	CtImage img;
} CtEngine;


CtContext*
Cute_newContext(CtImage* img);

void
Cute_run(CtContext* ctx, uint64_t procedure_id);

void
Cute_exec(CtContext* ctx);

#endif // CUTE_ENGINE_H