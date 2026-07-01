#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "containers/container.h"
#include "engine/error.h"

#include "context.h"




// Call Stack helpers

static inline void
ct_ctx_initStack(ctCallStack* s) {
    s->size     = 0;
    s->capacity = CUTE_CONF_CALLSTACK_SIZE;
}

static inline void 
ct_ctx_delStack(ctCallStack* s) {
    s->size    = 0;
    s->capacity = 0;
}

static inline void
ct_ctx_pushFrame(ctCallStack* s, ctCallFrame frame) {		
    s->frames[s->size++] = frame;
}

static inline ctCallFrame 
ct_ctx_popFrame(ctCallStack* s) {
    return s->frames[--s->size];
}

static inline ctCallFrame* 
ct_ctx_peekFrame(ctCallStack* s) {
    return &s->frames[s->size-1];
}


// Context methods

ctContext*
ct_ctx_new(ctImage* img, ctContainerManager* containers, uint32_t procedure_id) {
	ctContext* ctx = malloc(sizeof(ctContext));
	ctx->image = img;
	ctx->containers = containers;
	ctx->running = true;
	ctx->current_frame = NULL;
	ctx->has_error = false;
	ct_ctx_initStack(&ctx->callstack);
	ct_ctx_callProcedure(ctx, procedure_id, 0, 0, 0);
	return ctx;
}


void
ct_ctx_del(ctContext* ctx) {
	ct_ctx_delStack(&ctx->callstack);
}


void
ct_ctx_callProcedure(ctContext* ctx, uint32_t procedure_id, uint8_t arg_count, uint8_t arg_start_slot, uint8_t return_slot) {

	if (ctx->callstack.size >= ctx->callstack.capacity) {
		ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_RecursionDepth, "Max recursion depth reached."));
		return;
	};

	if (procedure_id >= ctx->image->header.procedure_count) {
		ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_ProcedureError, "Invalid procedure ID called."));
		return;
	}

	ctCallFrame frame;
	frame.procedure_id = procedure_id;
	frame.return_ip = ctx->ip;
	frame.return_value_slot = return_slot;
	frame.args_count = arg_count;
	memset(frame.file.types, 0, CUTE_CONF_SLOT_COUNT);

	for (size_t i = arg_start_slot; i < arg_start_slot + arg_count; i++) {
		frame.file.atoms[i] = ctx->current_frame->file.atoms[i];
		frame.file.types[i] = ctx->current_frame->file.types[i];
	};
	
	ct_ctx_pushFrame(&ctx->callstack, frame);

	ctx->ip = ctx->image->procedure_table[procedure_id].bytecode_index;
	ctx->current_frame = ct_ctx_peekFrame(&ctx->callstack);

	CUTE_LOG(
		"context", 
		"Called procedure(%u) with %u arguments passed from previous frame's slot %d\n", 
		procedure_id, arg_count, arg_start_slot
	);
}


void
ct_ctx_returnProcedure(ctContext* ctx, ctAtom returned_atom, ctAtomType returned_atom_type) {

	ctCallFrame frame = ct_ctx_popFrame(&ctx->callstack);
	ctx->current_frame = ct_ctx_peekFrame(&ctx->callstack);
	ctx->ip = frame.return_ip;
	ct_ctx_storeAtom(ctx, frame.return_value_slot, returned_atom, returned_atom_type);

	CUTE_LOG("context", "Returned from procedure(%u) with return value: %s\n", frame.procedure_id, ct_atom_stringforms[returned_atom_type]);
}

inline void
ct_ctx_storeAtom(ctContext* ctx, uint8_t slot, ctAtom atom, ctAtomType type) {

	if (ctx->current_frame->file.types[slot] == ctAtomType_Container) {
		ct_containers_decRef(ctx->containers, ctx->current_frame->file.atoms[slot].as_container);
	};

	ctx->current_frame->file.atoms[slot] = atom;
	ctx->current_frame->file.types[slot] = type;

	if (type == ctAtomType_Container) {
		ct_containers_incRef(ctx->containers, atom.as_container);
	};
};


inline void
ct_ctx_loadAtom(ctContext* ctx, uint8_t slot, ctAtom* atom, ctAtomType* type) {
	*atom = ctx->current_frame->file.atoms[slot];
	*type = ctx->current_frame->file.types[slot];
};


inline void
ct_ctx_moveAtom(ctContext* ctx, uint8_t src_slot, uint8_t dest_slot) {

	if (ctx->current_frame->file.types[dest_slot] == ctAtomType_Container) {
		ct_containers_decRef(ctx->containers, ctx->current_frame->file.atoms[dest_slot].as_container);
	};

	ctx->current_frame->file.atoms[dest_slot] = ctx->current_frame->file.atoms[src_slot];
	ctx->current_frame->file.types[dest_slot] = ctx->current_frame->file.types[src_slot];

	if (ctx->current_frame->file.types[src_slot] == ctAtomType_Container) {
		ct_containers_incRef(ctx->containers, ctx->current_frame->file.atoms[src_slot].as_container);
	};
};


inline void
ct_ctx_throwError(ctContext* ctx, ctError error) {
	ctx->running = false;
	ctx->has_error = true;
	ctx->error = error;
}