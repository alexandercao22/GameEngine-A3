#include "TestCases.h"
#include "Interface.h"
#include "PackageManager.h"
#include "Scene.h"
#include "ResourceManager.h"

#include "raylib.h"
#include <chrono>

#include <filesystem>

int main() {
	//Interface interface;

	ResourceManager::Instance().Init();

	Scene scene;
	scene.Init();

	while (!WindowShouldClose()) {
		//interface.Update();

		scene.Update();
		scene.RenderUpdate();
	}

	CloseWindow();

	// Run tests
	//PoolVSOS();
	//TestAll();

	// PackageManager tests
	//PackageManager pm;
	//pm.Pack("../GameEngine/Resources/level2", "../GameEngine/Resources");
	////pm.Unpack("../GameEngine/Resources/level1.gepak", "../GameEngine/Resources");
	//pm.MountPackage("../GameEngine/Resources/level1.gepak");
	//pm.MountPackage("../GameEngine/Resources/level2.gepak");
	//AssetData asset;
	//pm.LoadAssetByPath("293088.png", asset);
	//pm.LoadAssetByGuid("f0ea19eb-e072-403b-bda6-fd6a78c8b92a", asset);
	//pm.UnmountPackage();
	//pm.UnmountPackage("level1");
	//pm.UnmountPackage();

	//PoolVSOS();
	//TestAll();
	return 0;
}