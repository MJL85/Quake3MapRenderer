#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"
#include "engine/engine.h"

#include "math/matrix.h"

int main(int argc, char** argv) {
	printf(STARTUP_BANNER "\n\n");

	if (!g_engine.init())
		return 0;

	g_engine.exec();
	g_engine.shutdown();

	return 0;
}
