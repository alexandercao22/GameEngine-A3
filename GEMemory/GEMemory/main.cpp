#include "TestCases.h"
#include "Interface.h"

#include "raylib.h"

int main() {
	Interface interface;

	while (!WindowShouldClose()) {
		interface.Update();
	}

	CloseWindow();

	// Run tests
	//PoolVSOS();
	//TestAll();
	return 0;
}