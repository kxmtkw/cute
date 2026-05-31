#include "CuteEngine.h"

int
main() {
	
	ctEngine engine;
	ct_engine_init(&engine);
	ct_engine_loadFile(&engine, "demo.cti");
	ct_engine_run(&engine);
	ct_engine_end(&engine);
	return 0;
}