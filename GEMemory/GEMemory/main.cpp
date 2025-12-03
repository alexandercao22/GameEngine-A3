#include "TestCases.h"
#include "Interface.h"

#include "raylib.h"

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
	Model mushroom = LoadModel("mushroom.obj");
	Model cubeModel = LoadModelFromMesh(cubeMesh);
	

	while (!WindowShouldClose()) {
		//interface.Update();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		BeginMode3D(camera);

		//DrawModel(cubeModel, { 0.0f, 1.0f, 0.0f }, 1.0f, BLUE);
		DrawModel(mushroom, {3,2,0}, 1.0f, RED);
		DrawGrid(20, 1.0f); // Bara för orientering

		EndMode3D();
		// --- 3D Mode slutar här ---
		EndDrawing();

		//DrawModel(LoadModelFromMesh(GenMeshCube(5, 4, 5)), Vector3{ 0,0,-1 }, 1.0f, BLUE);

	}
	UnloadModel(cubeModel);
	CloseWindow();

	//PoolVSOS();


	//TestAll();
	return 0;
}