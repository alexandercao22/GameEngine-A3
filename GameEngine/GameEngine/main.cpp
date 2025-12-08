#include "TestCases.h"
#include "Interface.h"
#include "PackageManager.h"
#include "ResourceManager.h"

#include "raylib.h"
#include <chrono>

#include <filesystem>
#include <thread>

std::thread::id mainThreadID;

void threadFunction(MeshResource** resPtr) {
	std::this_thread::sleep_for(std::chrono::seconds(1));
	ResourceManager& RM = ResourceManager::Instance();
	
	*resPtr = dynamic_cast<MeshResource *>(RM.Load("69f74b0d-b87d-44b0-bb89-4fb94b5243c8"));
}

int main() {
	//ResourceManager RM;
	InitWindow(1280, 720, "3D Test");
	mainThreadID = std::this_thread::get_id();
	ResourceManager::Instance().Init();
	const int numThreads = 4;
	std::vector<std::thread> threads;
	std::vector<MeshResource*> resources;
	for (int i = 0; i < numThreads; i++) {
		MeshResource* res = nullptr;
		resources.emplace_back(res);
	}
	for (int i = 0; i < numThreads; i++) {
		threads.emplace_back(threadFunction, &resources[i]);

	}
	for (auto& t : threads) {
		t.join();
	}

	MeshResource* res1 = dynamic_cast<MeshResource*>(ResourceManager::Instance().Load("69f74b0d-b87d-44b0-bb89-4fb94b5243c8"));
	resources.emplace_back(res1);
	std::cout << res1->GetRef() << std::endl;
	std::cout << "Alla trådar är klara! " << std::endl;


	// --- 1) Kamera ---
	Camera3D camera = { 0 };
	camera.position = { 0.0f, 5.0f, 10.0f }; // Var kameran är
	camera.target = { 0.0f, 0.0f, 0.0f };  // Vart kameran tittar
	camera.up = { 0.0f, 1.0f, 0.0f };  // Up-vector
	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		// Check if something is in the RM.CPUReadyResources

		auto t0 = std::chrono::high_resolution_clock::now();

		//interface.Update();
		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(camera);

		for (int i = 0; i < numThreads; i++) {
			DrawModel(resources[i]->GetModel(), { -4.0f + i * 2, 2, 0 }, 1.0f, RED);
		}
		
		

		DrawModel(res1->GetModel(), { 3,2,0 }, 1.0f, RED);
		//std::cout << res1->GetRef() << std::endl;
		/*DrawModel(meshroom2->GetModel(), {-3,2,0}, 1.0f, BLUE);
		*/
		DrawGrid(20, 1.0f); // Bara för orientering

		EndMode3D();
		// --- 3D Mode slutar här ---
		EndDrawing();


		

	}

	CloseWindow();


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
	pm.Pack("../GameEngine/Resources/level2", "../GameEngine/Resources");
	//pm.Unpack("../GameEngine/Resources/level1.gepak", "../GameEngine/Resources");
	pm.MountPackage("../GameEngine/Resources/level1.gepak");
	pm.MountPackage("../GameEngine/Resources/level2.gepak");
	AssetData asset;
	pm.LoadAssetByPath("293088.png", asset);
	pm.LoadAssetByGuid("f0ea19eb-e072-403b-bda6-fd6a78c8b92a", asset);
	pm.UnmountPackage();
	pm.UnmountPackage("level1");
	pm.UnmountPackage();

	//PoolVSOS();
	//TestAll();
	return 0;
}