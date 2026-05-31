#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "engine/failure.h"

#include "context.h"


static ctAtomFile*
ct_ctx_allocAtomFile(uint32_t count) {
	ctAtomFile* file = malloc(
		sizeof(ctAtom) * count + 
		sizeof(ctAtomTypeSize) * count +
		sizeof(uint32_t)
	);
	file->size = count;
	return file;
}


static void
ct_ctx_freeAtomFile(ctAtomFile* file) {
	if (file != NULL) {
		free(file);
	}
}


static void
ct_ctx_initStack(ctCallStack* s) {
    s->size     = 0;
    s->capacity = CUTE_CONF_CALLSTACK_SIZE;
}


static void 
ct_ctx_delStack(ctCallStack* s) {

	for (uint32_t i = 0; i < s->size; i++) {
		ct_ctx_freeAtomFile(s->frames[i].locals);
	};

    s->size    = 0;
    s->capacity = 0;
}


static void
ct_ctx_pushFrame(ctCallStack* s, ctCallFrame frame) {		
    s->frames[s->size++] = frame;
}


static ctCallFrame 
ct_ctx_popFrame(ctCallStack* s) {
    return s->frames[--s->size];
}


static ctCallFrame* 
ct_ctx_peekFrame(ctCallStack* s) {
    return &s->frames[s->size-1];
}


ctContext*
ct_ctx_new(ctImage* img, uint64_t procedure_id) {
	ctContext* ctx = malloc(sizeof(ctContext));
	ctx->image = img;
	ctx->running = true;
	ctx->current_frame = NULL;
	ctx->has_failure = false;
	ct_ctx_initStack(&ctx->callstack);
	ct_ctx_callProcedure(ctx, procedure_id);
	return ctx;
}


void
ct_ctx_del(ctContext* ctx) {
	ct_ctx_delStack(&ctx->callstack);
}


void
ct_ctx_callProcedure(ctContext* ctx, uint64_t procedure_id) {

	if (ctx->callstack.size >= ctx->callstack.capacity) {
		ct_ctx_reportFailure(ctx, (ctFailure){"Max recursion depth reached."});
		return;
	};

	ctCallFrame frame;
	frame.procedure_id = procedure_id;
	frame.locals = ct_ctx_allocAtomFile(ctx->image->procedure_table[procedure_id].locals_size);
	frame.return_ip = ctx->ip;
	ct_ctx_pushFrame(&ctx->callstack, frame);


	ctx->ip = ctx->image->procedure_table[procedure_id].bytecode_index;
	ctx->current_frame = ct_ctx_peekFrame(&ctx->callstack);	
}


void
ct_ctx_returnProcedure(ctContext* ctx) {

	ctCallFrame frame = ct_ctx_popFrame(&ctx->callstack);
	ct_ctx_freeAtomFile(frame.locals);

	ctx->ip = frame.return_ip;
	ctx->current_frame = ct_ctx_peekFrame(&ctx->callstack);
}


void 
ct_ctx_reportFailure(ctContext* ctx, ctFailure failure) {
	ctx->running = false;
	ctx->has_failure = true;
	ctx->failure = failure;
}