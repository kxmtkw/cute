

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <stdint.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "containers/container.h"
#include "engine/error.h"


typedef struct {
	uint64_t        size;
	ctAtom*         atoms;
	ctAtomTypeSize* types;
} ctAtomFile;


typedef struct {
	ctAtom         atoms[CUTE_CONF_REGISTER_COUNT];
	ctAtomTypeSize types[CUTE_CONF_REGISTER_COUNT];
} ctRegisterFile;


typedef struct {
	uint64_t    procedure_id;
	uint64_t    return_ip;
	ctAtomFile  locals;
} ctCallFrame;


typedef struct {
	ctCallFrame  frames[CUTE_CONF_CALLSTACK_SIZE];
	uint64_t     size;
	uint64_t     capacity;
} ctCallStack;



typedef struct {
	ctImage*       image;
	ctContainerManager* containers;
	uint64_t       ip;
	ctRegisterFile registers;
	ctCallStack    callstack;
	ctCallFrame*   current_frame;
	bool           running;
	bool           has_error;
	ctError        error;
	uint8_t        exit_code;
} ctContext;


// Create a new context. Requires the image to be ran and the starting procedure.
ctContext*
ct_ctx_new(ctImage* img, ctContainerManager* containers, uint32_t procedure_id);

// Free the context and its resources.
void
ct_ctx_del(ctContext* ctx);

// Setup a callframe and allocated local variables for a procedure.
void
ct_ctx_callProcedure(ctContext* ctx, uint32_t procedure_id);

// Return from the last called procedure.
void
ct_ctx_returnProcedure(ctContext* ctx);

void
ct_ctx_throwError(ctContext* ctx, ctError error);


// Executes a context, the heart of the engine.
void
ct_ctx_exec(ctContext* ctx);



#endif // ENGINE_CONTEXT_H