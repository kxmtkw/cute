
#ifndef CUTE_ENGINE_H
#define CUTE_ENGINE_H

#include <stdint.h>
#include "CuteByte.h"


typedef struct {
	uint64_t procedure_id;
	uint32_t locals_count;
	uint64_t* local_atoms;
} CtCallFrame;

typedef struct {
	CtCallFrame* call_stack;
	uint64_t size;
} CtCallStack;

void CtCallStack_init(CtCallStack* stack);
void CtCallStack_del(CtCallStack* stack);
void CtCallStack_push(CtCallStack* stack, CtCallFrame frame);
CtCallFrame CtCallStack_pop(CtCallStack* stack);

typedef struct {
	CtImage* image;
	CtCallStack call_stack;
	int64_t	ip;
	int64_t registers[16];
} CtContext;

typedef struct {
	CtContext ctx;
	CtImage img;
} CtEngine;

#endif // CUTE_ENGINE_H