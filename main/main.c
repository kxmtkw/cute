#include "CuteByte.h"
#include <stdio.h>


void CtImage_print(const CtImage* img) {
	printf("=== CtImage ===\n");
	printf("magic:       0x%08X\n", img->header.magic_id);
	printf("proc count:  %lu\n",   img->header.procedure_count);
	printf("instr count: %lu\n",   img->header.instruction_count);

	printf("\n--- Procedures ---\n");
	for (uint64_t i = 0; i < img->header.procedure_count; i++) {
		CtImageProcedure* p = &img->procedure_table[i];
		printf("  [%lu] id=%lu bytecode_index=%lu locals=%u\n",
			i, p->id, p->bytecode_index, p->locals_size);
	}

	printf("\n--- Instructions ---\n");
	for (uint64_t i = 0; i < img->header.instruction_count; i++) {
		printf("  [%04lu] 0x%02X\n", i, img->instruction_pool[i]);
	}
}

int
main() {

	// instructions: load 5, load 3, add, out, halt
	CtInstructionSize instructions[] = {
		instrLoad, 0, 0,   // load const[0] (5) -> R0
		instrLoad, 1, 1,   // load const[1] (3) -> R1
		instrAddI, 0, 1,   // R0 = R0 + R1
		instrOut,  0, 2,   // out R0 as int
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

	CtImage_print(&img);
	CtImageCode code = CtImage_write(&img, "test.bin");

	switch (code) {
		case CtImageCode_Success:
			printf("Image written successfully.\n");
			break;
		case CtImageCode_FileNotFound:
			printf("Error: File not found.\n");
			break;
		case CtImageCode_ReadWriteFailure:
			printf("Error: Read/write failure.\n");
			break;
		case CtImageCode_InvalidImage:
			printf("Error: Invalid image format.\n");
			break;
	}
   
	

	CtImage img2 = {0};
	code = CtImage_read(&img2, "test.bin");
	CtImage_print(&img2);
	switch (code) {
		case CtImageCode_Success:
			printf("Image written successfully.\n");
			break;
		case CtImageCode_FileNotFound:
			printf("Error: File not found.\n");
			break;
		case CtImageCode_ReadWriteFailure:
			printf("Error: Read/write failure.\n");
			break;
		case CtImageCode_InvalidImage:
			printf("Error: Invalid image format.\n");
			break;
	}
   
	
	return 0;
}