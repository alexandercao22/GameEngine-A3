#include "TestCases.h"
#include "Interface.h"
#include "PackageManager.h"

#include "raylib.h"

#include <filesystem>

int main() {
	//Interface interface;

	//while (!WindowShouldClose()) {
	//	interface.Update();
	//}

	//CloseWindow();

	//PoolVSOS();
	//TestAll();


	// PackageManager tests
	PackageManager pm;
	//pm.Pack("../GEMemory/Resources/level1", "../GEMemory/Resources");
	pm.Unpack("../GEMemory/Resources/level1.gepak", "../GEMemory/Resources");
	pm.MountPackage("../GEMemory/Resources/level1.gepak");
	AssetData asset;
	pm.LoadAsset("293088.png", "level1", asset);
	pm.UnmountPackage("level1");

	return 0;
}