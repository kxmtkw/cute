

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <stdint.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "engine/failure.h"

typedef struct {
	ctAtom*         atoms;
	ctAtomTypeSize* types;
	uint32_t        size;
} ctAtomFile;


typedef struct {
	ctAtom         atoms[CUTE_CONF_REGISTER_COUNT];
	ctAtomTypeSize types[CUTE_CONF_REGISTER_COUNT];
} ctRegisterFile;


typedef struct {
	uint64_t    procedure_id;
	uint64_t    return_ip;
	ctAtomFile* locals;
} ctCallFrame;


typedef struct {
	ctCallFrame  frames[CUTE_CONF_CALLSTACK_SIZE];
	uint64_t     size;
	uint64_t     capacity;
} ctCallStack;


typedef struct {
	char* msg;
} ctFailure;


typedef struct {
	ctImage*       image;
	uint64_t       ip;
	ctRegisterFile registers;
	ctCallStack    callstack;
	ctCallFrame*   current_frame;
	bool           running;
	bool           has_failure;
	ctFailure      failure;
} ctContext;


// Create a new context. Requires the image to be ran and the starting procedure.
ctContext*
ct_ctx_new(ctImage* img, uint64_t procedure_id);

// Free the context and its resources.
void
ct_ctx_del(ctContext* ctx);

// Setup a callframe and allocated local variables for a procedure.
void
ct_ctx_callProcedure(ctContext* ctx, uint64_t procedure_id);

// Return from the last called procedure.
void
ct_ctx_returnProcedure(ctContext* ctx);

void
ct_ctx_reportFailure(ctContext* ctx, ctFailure failure);


// Executes a context, the heart of the engine.
void
ct_ctx_exec(ctContext* ctx);



#endif // ENGINE_CONTEXT_H