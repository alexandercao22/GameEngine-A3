#include "TestCases.h"
#include "Interface.h"
#include "ResourceManager.h"
#include "MeshResource.h"

#include "raylib.h"
#include <chrono>

int main() {
	//Interface interface;

	InitWindow(1280, 720, "3D Test");

	// --- 1) Kamera ---
	Camera3D camera = { 0 };
	camera.position = { 0.0f, 5.0f, 10.0f }; // Var kameran är
	camera.target = { 0.0f, 0.0f, 0.0f };  // Vart kameran tittar
	camera.up = { 0.0f, 1.0f, 0.0f };  // Up-vector
	camera.fovy = 60.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(60);

	// --- 2) En mesh-kub + model ---
	Mesh cubeMesh = GenMeshCube(2.0f, 2.0f, 2.0f);
	Model mushroom = LoadModel("Resources/mushroom.obj");
	Model cubeModel = LoadModelFromMesh(cubeMesh);

	// Create a ResourceManager Instance
	ResourceManager RM;
	RM.Init();
	//std::string guid = RM.SaveGUID("Resources/mushroom.obj");
	//Resource* mushroom1 = RM.Load("69f74b0d-b87d-44b0-bb89-4fb94b5243c8");
	MeshResource* meshroom1 = dynamic_cast<MeshResource*>(RM.Load("69f74b0d-b87d-44b0-bb89-4fb94b5243c8"));

	MeshResource* meshroom2 = dynamic_cast<MeshResource*>(RM.Load("69f74b0d-b87d-44b0-bb89-4fb94b5243c8"));

	double time = 0;

	while (!WindowShouldClose()) {
		auto t0 = std::chrono::high_resolution_clock::now();
		
		//interface.Update();
		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(camera);
		
		DrawModel(meshroom1->GetModel(), {3,2,0}, 1.0f, RED);
		DrawModel(meshroom2->GetModel(), {-3,2,0}, 1.0f, BLUE);
		//DrawModel(cubeModel, { 0.0f, 1.0f, 0.0f }, 1.0f, BLUE);
		DrawGrid(20, 1.0f); // Bara för orientering

		EndMode3D();
		// --- 3D Mode slutar här ---
		EndDrawing();
		

		//DrawModel(LoadModelFromMesh(GenMeshCube(5, 4, 5)), Vector3{ 0,0,-1 }, 1.0f, BLUE);
		auto t1 = std::chrono::high_resolution_clock::now(); 
		std::chrono::duration<double> duration = t1 - t0;
		time += duration.count();

		//std::cout << "mesh1 " << meshroom1->GetRef() << std::endl;
		std::cout << "mesh2 " << meshroom2->GetRef() << std::endl;

	/*	if (time >= 5 && time < 6) {

			if (!RM.UnLoad("69f74b0d-b87d-44b0-bb89-4fb94b5243c8")) {
				std::cerr << "ERROR: Trying to UnLoad already unloaded content" << std::endl;
			}
			time = 6;
		}
		if (time >= 10) {
			if (!RM.UnLoad("69f74b0d-b87d-44b0-bb89-4fb94b5243c8")) {
				std::cerr << "ERROR: Trying to UnLoad already unloaded content" << std::endl;
			}
		}*/

	}


	
	UnloadModel(cubeModel);
	CloseWindow();

	//PoolVSOS();


	//TestAll();
	return 0;
}