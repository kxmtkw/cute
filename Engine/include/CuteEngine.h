
#ifndef CUTE_ENGINE_H
#define CUTE_ENGINE_H

#include <stdint.h>
#include "CuteAtom.h"
#include "CuteInstr.h"

#include "engine/context.h"
#include "engine/failure.h"

typedef struct {
	ctImage    image;
	ctContext* ctx;
} ctEngine;


// Intialize the engine
void
ct_engine_init(ctEngine* engine);

// End the engine and free all resources
void
ct_engine_end(ctEngine* engine);

// Load an image file. For now, only one image can be loaded.
void
ct_engine_loadFile(ctEngine* engine, const char* filepath);

// Run the engine with the loaded image file
void
ct_engine_run(ctEngine* engine);

//
void
ct_engine_reportFailure(ctFailure failure);

#endif // CUTE_ENGINE_H