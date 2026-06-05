#include "CuteEngine.h"

int
main(int argc, char** argv) {
	
	if (argc < 2) {
		printf("Usage: %s <image_file>\n", argv[0]);
		return 2;
	}

	ctEngine engine;
	ct_engine_init(&engine);
	ct_engine_loadFile(&engine, argv[1]);
	ct_engine_run(&engine);
	ct_engine_end(&engine);
	return 0;
}