#include "Scene.h"

#include "imgui.h"
#include "rlImGui.h"

bool Scene::Init(unsigned int width, unsigned int height)
{
	InitWindow(width, height, "Game Engine Assignment 3");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	_resourceManager.Init();

	// Initialize camera
	_camera.position = { 0.0f, 5.0f, 10.0f };
	_camera.target = { 0.0f, 0.0f, 0.0f };
	_camera.up = { 0.0f, 1.0f, 0.0f };
	_camera.fovy = 90.0f;
	_camera.projection = CAMERA_PERSPECTIVE;

	_cubeMesh = GenMeshCube(2.0f, 2.0f, 2.0f);
	_duck = LoadModel("Resources/mushroom.obj");
	_cubeModel = LoadModelFromMesh(_cubeMesh);

	return true;
}

void Scene::Update()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	rlImGuiBegin();
	BeginMode3D(_camera);

	if (IsKeyPressed(KEY_C)) {
		_showCursor = !_showCursor;
		if (_showCursor) {
			EnableCursor();
		}
		else {
			DisableCursor();
		}
	}

	if (!_showCursor) {
		UpdateCamera(&_camera, CAMERA_FREE);
	}

	DrawModel(_cubeModel, Vector3{ 0,0,-1 }, 1.0f, BLUE);
	DrawModel(_duck, { 3,2,0 }, 1.0f, RED);
	DrawGrid(20, 1.0f);

	EndMode3D();
	rlImGuiEnd();
	EndDrawing();
}
