#include "SceneManager.h"

#include "imgui.h"
#include "rlImGui.h"

#include "ResourceManager.h"
#include "MeshResource.h"

bool SceneManager::Init(unsigned int width, unsigned int height)
{
	InitWindow(width, height, "Game Engine Assignment 3");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	// Initialize camera
	_camera.position = { 0.0f, 5.0f, 10.0f };
	_camera.target = { 0.0f, 0.0f, 0.0f };
	_camera.up = { 0.0f, 1.0f, 0.0f };
	_camera.fovy = 90.0f;
	_camera.projection = CAMERA_PERSPECTIVE;

	return true;
}

bool SceneManager::Update()
{
	return true;
}

bool SceneManager::RenderUpdate()
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

	for (std::string guid : _GUIDs) {
		if (ResourceManager::Instance().GetGUIDType(guid) == "Mesh") {
			MeshResource *mesh = (MeshResource *)ResourceManager::Instance().Load(guid);
			DrawModel(mesh->GetModel(), { 0, 0, 0 }, 1.0f, WHITE);
		}
	}

	DrawGrid(20, 1.0f);

	EndMode3D();
	rlImGuiEnd();
	EndDrawing();

	return true;
}
