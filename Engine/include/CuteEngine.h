
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
	CtImage* image;
	CtCallFrame call_frame;
	uint64_t	ip;
	CtAtom registers[32];
} CtContext;

#define CT_VALIDREGISTER(r) assert((r) < sizeof(ctx->registers) / sizeof(ctx->registers[0]))

#define CT_BINARYOP()

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