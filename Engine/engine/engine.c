
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "CuteAtom.h"
#include "CuteInstr.h"
#include "CuteEngine.h"

#include "containers/container.h"
#include "context.h"


void
ct_engine_init(ctEngine* engine) {
	CUTE_LOG("engine", "vroom vroom\n");
	engine->ctx = NULL;
	ct_containers_init(&engine->manager);
}

// End the engine and free all resources
void
ct_engine_end(ctEngine* engine) {
	ct_ctx_del(engine->ctx);
	ct_containers_end(&engine->manager);
	ct_image_free(&engine->image);

	CUTE_LOG("engine", "Ending engine.\n");
}

// Load an image file. For now, only one image can be loaded.
void
ct_engine_loadFile(ctEngine* engine, const char* filepath) {
	CUTE_LOG("engine", "Loading image file: %s\n", filepath);
	ctImageCode code = ct_image_read(&engine->image, filepath);
}

// Run the engine with the loaded image file
void
ct_engine_run(ctEngine* engine) {
	engine->ctx = ct_ctx_new(&engine->image, &engine->manager, 0);
	ct_ctx_exec(engine->ctx);
	
	if (engine->ctx->has_error) {
		ct_error_print(engine->ctx->error);
	}

	engine->exit_code = engine->ctx->exit_code;
}
