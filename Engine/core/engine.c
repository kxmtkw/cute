#include <stdlib.h>

#include "CuteByte.h"
#include "CuteEngine.h"

CtContext*
Cute_newContext(CtImage *img) {
	CtContext* ctx = (CtContext*) malloc(sizeof(CtContext));
	ctx->ip = 0;
	ctx->image = img;
	return ctx;
}

void
Cute_run(CtContext* ctx, uint64_t procedure_id) {

	ctx->call_frame.procedure_id = procedure_id;
	ctx->call_frame.locals_count = ctx->image->procedure_table[procedure_id].locals_size;
	ctx->call_frame.local_atoms = (CtAtom*) malloc(sizeof(CtAtom) * ctx->call_frame.locals_count);

	ctx->ip = ctx->image->procedure_table[procedure_id].bytecode_index;

	Cute_exec(ctx);
}