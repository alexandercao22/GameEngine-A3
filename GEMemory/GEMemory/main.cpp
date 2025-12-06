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

	// Run tests
	//PoolVSOS();
	//TestAll();

	// PackageManager tests
	PackageManager pm;
	pm.Pack("../GEMemory/Resources/level2", "../GEMemory/Resources");
	//pm.Unpack("../GEMemory/Resources/level1.gepak", "../GEMemory/Resources");
	pm.MountPackage("../GEMemory/Resources/level1.gepak");
	pm.MountPackage("../GEMemory/Resources/level2.gepak");
	AssetData asset;
	pm.LoadAssetByPath("293088.png", asset);
	pm.LoadAssetByGuid("f0ea19eb-e072-403b-bda6-fd6a78c8b92a", asset);
	pm.UnmountPackage();
	pm.UnmountPackage("level1");
	pm.UnmountPackage();

	return 0;
}