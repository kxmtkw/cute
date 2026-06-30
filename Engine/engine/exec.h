
#ifndef ENGINE_EXEC_H
#define ENGINE_EXEC_H

#include <stdint.h>
#include <stdio.h>

#include "engine/context.h"

// Executes a context, the heart of the engine.
void
ct_exec(ctContext* ctx);

#endif // ENGINE_EXEC_H