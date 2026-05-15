#include "CuteByte.h"
#include <stdio.h>
#include <stdlib.h>


void 
CtImage_init(CtImage *img)
{
	img->header.magic_id = 0;
	img->header.instruction_count = 0;
	img->header.procedure_count = 0;
	img->procedure_table = NULL;
	img->instruction_pool = NULL;
};


void 
CtImage_del(CtImage* img)
{
	img->header.instruction_count = 0;
	img->header.procedure_count = 0;

	if (img->procedure_table != NULL) {
		free(img->procedure_table);
	}

	if (img->instruction_pool != NULL) {
		free(img->instruction_pool);
	}
};


CtImageCode 
CtImage_write(CtImage *img, const char *filepath) {

	FILE *fp = fopen(filepath, "wb");
	if (!fp) {return CtImageCode_FileNotFound;}

	img->header.magic_id = CtMagicId;

	u_int32_t items_written;

	items_written = fwrite(&img->header, sizeof(CtImageHeader), 1, fp);
	if (items_written != 1) {return CtImageCode_ReadWriteFailure;}

	items_written = fwrite(img->procedure_table, sizeof(CtImageProcedure), img->header.procedure_count, fp);
	if (items_written != img->header.procedure_count) {return CtImageCode_ReadWriteFailure;}

	items_written = fwrite(img->instruction_pool, sizeof(CtInstructionSize), img->header.instruction_count, fp);
	if (items_written != img->header.instruction_count) {return CtImageCode_ReadWriteFailure;}

	fclose(fp);
	return CtImageCode_Success;
}


CtImageCode 
CtImage_read(CtImage *img, const char *filepath) {

	FILE *fp = fopen(filepath, "rb");
	if (!fp) {return CtImageCode_FileNotFound;}

	uint32_t items_read;

	items_read = fread(&img->header, sizeof(CtImageHeader), 1, fp);
	if (items_read != 1) {return CtImageCode_ReadWriteFailure;}

	if (img->header.magic_id != CtMagicId) {
		return CtImageCode_InvalidImage;
	}

	img->procedure_table = malloc(sizeof(CtImageProcedure) * img->header.procedure_count);
	items_read = fread(img->procedure_table, sizeof(CtImageProcedure), img->header.procedure_count, fp);
	if (items_read != img->header.procedure_count) {return CtImageCode_ReadWriteFailure;}

	img->instruction_pool = malloc(sizeof(CtInstructionSize) * img->header.instruction_count);
	items_read = fread(img->instruction_pool, sizeof(CtInstructionSize), img->header.instruction_count, fp);
	if (items_read != img->header.instruction_count) {return CtImageCode_ReadWriteFailure;}
	
	fclose(fp);
	return CtImageCode_Success;
};


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
		printf("  [%04lu] 0x%02X %d\n", i, img->instruction_pool[i], img->instruction_pool[i]);
	}
}