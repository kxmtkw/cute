

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <stdint.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"
#include "containers/container.h"
#include "engine/error.h"


typedef struct {
	ctAtom         atoms[CUTE_CONF_SLOT_COUNT];
	ctAtomTypeSize types[CUTE_CONF_SLOT_COUNT];
} ctAtomFile;



typedef struct {
	uint32_t            procedure_id;
	uint64_t            return_ip;
	uint8_t             args_count;
	uint8_t             return_value_slot;
	ctAtomFile          file;
} ctCallFrame;


typedef struct {
	ctAtomFile   file;
	ctCallFrame  frames[CUTE_CONF_CALLSTACK_SIZE];
	uint32_t     size;
	uint32_t     capacity;
} ctCallStack;


typedef struct {
	ctImage*            image;
	ctContainerManager* containers;
	uint64_t            ip;
	ctCallStack         callstack;
	ctCallFrame*        current_frame;
	double              cmp_diff;
	bool                running;
	bool                has_error;
	ctError             error;
	uint8_t            exit_code;
} ctContext;


// Create a new context. Requires the image to be ran and the starting procedure.
ctContext*
ct_ctx_new(ctImage* img, ctContainerManager* containers, uint32_t procedure_id);

// Free the context and its resources.
void
ct_ctx_del(ctContext* ctx);

// Setup a callframe and allocated local variables for a procedure.
void
ct_ctx_callProcedure(ctContext* ctx, uint32_t procedure_id, uint32_t arg_count, uint8_t arg_start_slot, uint8_t return_slot);

// Return from the last called procedure.
void
ct_ctx_returnProcedure(ctContext* ctx, ctAtom returned_atom, ctAtomType returned_atom_type);

// Store an atom in the current call frame
void
ct_ctx_storeAtom(ctContext* ctx, uint8_t slot, ctAtom atom, ctAtomType type);

// Load an atom from the current call frame
void
ct_ctx_loadAtom(ctContext* ctx, uint8_t slot, ctAtom* atom, ctAtomType* type);

// Move an atom within the callframe
void
ct_ctx_moveAtom(ctContext* ctx, uint8_t src_slot, uint8_t dest_slot);

// Throw an internal error
void
ct_ctx_throwError(ctContext* ctx, ctError error);



#endif // ENGINE_CONTEXT_H