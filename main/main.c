#include "CuteByte.h"
#include "CuteEngine.h"
#include <stdio.h>


int
main() {

	// instructions: load 5, load 3, add, out, halt
	CtInstructionSize instructions[] = {
		instrLoadI, 0,0,0,0,0,   // load const[0] (5) -> R0
		instrLoadI, 1,0,0,0,1,   // load const[0] (5) -> R0
		instrAddI, 0,1,
		instrOut,  0, 2,   // out R0 as int
		instrJmp, 12,
		instrHalt, 0,      // halt
	};

	CtImageProcedure procs[] = {
		{ .id = 0, .bytecode_index = 0, .locals_size = 2 }
	};

	CtImage img = {
		.header = {
			.procedure_count         = 1,
			.instruction_count       = sizeof(instructions) / sizeof(instructions[0]),
		},
		.procedure_table  = procs,
		.instruction_pool = instructions,
	};

	CtImage_write(&img, "test.bin");

	CtImage_init(&img);
	CtImage_read(&img, "test.bin");
	CtContext* ctx = Cute_newContext(&img);
	Cute_run(ctx, 0);
   
	return 0;
}