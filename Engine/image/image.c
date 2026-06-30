#include <stdio.h>
#include <stdlib.h>

#include "CuteInstr.h"



void 
ct_image_free(ctImage* img)
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


ctImageCode 
ct_image_write(ctImage *img, const char *filepath) {

	FILE *fp = fopen(filepath, "wb");
	if (!fp) {return ctImageCode_FileNotFound;}

	img->header.magic_id = ctMagicId;

	u_int32_t items_written;

	items_written = fwrite(&img->header, sizeof(ctImageHeader), 1, fp);
	if (items_written != 1) {return ctImageCode_ReadWriteFailure;}

	items_written = fwrite(img->procedure_table, sizeof(ctImageProcedure), img->header.procedure_count, fp);
	if (items_written != img->header.procedure_count) {return ctImageCode_ReadWriteFailure;}

	items_written = fwrite(img->instruction_pool, sizeof(ctInstructionSize), img->header.instruction_count, fp);
	if (items_written != img->header.instruction_count) {return ctImageCode_ReadWriteFailure;}

	fclose(fp);
	return ctImageCode_Success;
}


ctImageCode 
ct_image_read(ctImage *img, const char *filepath) {

	FILE *fp = fopen(filepath, "rb");
	if (!fp) {return ctImageCode_FileNotFound;}

	uint32_t items_read;

	items_read = fread(&img->header, sizeof(ctImageHeader), 1, fp);
	if (items_read != 1) {return ctImageCode_ReadWriteFailure;}

	if (img->header.magic_id != ctMagicId) {
		return ctImageCode_InvalidImage;
	}

	img->procedure_table = malloc(sizeof(ctImageProcedure) * img->header.procedure_count);
	items_read = fread(img->procedure_table, sizeof(ctImageProcedure), img->header.procedure_count, fp);
	if (items_read != img->header.procedure_count) {return ctImageCode_ReadWriteFailure;}

	img->instruction_pool = malloc(sizeof(ctInstructionSize) * img->header.instruction_count);
	items_read = fread(img->instruction_pool, sizeof(ctInstructionSize), img->header.instruction_count, fp);
	if (items_read != img->header.instruction_count) {return ctImageCode_ReadWriteFailure;}
	
	fclose(fp);
	return ctImageCode_Success;
};
