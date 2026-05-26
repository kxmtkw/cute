#include "CuteByte.h"
#include "CuteEngine.h"
#include <stdio.h>


int
main() {
	CtImage img;
	CtImage_init(&img);
	CtImage_read(&img, "demo.cti");

	CtContext* ctx = Cute_newContext(&img);
	Cute_run(ctx, 0);
   
	return 0;
}