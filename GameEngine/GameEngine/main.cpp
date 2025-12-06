#include "TestCases.h"
#include "Interface.h"
#include "ResourceManager.h"
#include "SceneManager.h"

#include "raylib.h"
#include <chrono>

int main() {
	//Interface interface;

	ResourceManager::Instance().Init();

	SceneManager scene;
	scene.Init();

	while (!WindowShouldClose()) {
		scene.Update();
		scene.RenderUpdate();
	}

	CloseWindow();

	//PoolVSOS();
	//TestAll();
	return 0;
}