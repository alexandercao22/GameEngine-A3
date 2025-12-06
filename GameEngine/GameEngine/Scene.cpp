#include "Scene.h"

#include "imgui.h"
#include "rlImGui.h"

#include "ResourceManager.h"
#include "MeshResource.h"

bool Scene::Init(unsigned int width, unsigned int height)
{
	_width = width;
	_height = height;

	InitWindow(_width, _height, "Game Engine Assignment 3");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	// Initialize camera
	_camera.position = { 0.0f, 5.0f, 10.0f };
	_camera.target = { 0.0f, 0.0f, 0.0f };
	_camera.up = { 0.0f, 1.0f, 0.0f };
	_camera.fovy = 90.0f;
	_camera.projection = CAMERA_PERSPECTIVE;

	Entity ent;
	ent.AddGUID("69f74b0d-b87d-44b0-bb89-4fb94b5243c8");
	ent.GetTransform()->translation.x = 0.0f;
	_entities.push_back(ent);
	ent.GetTransform()->translation.x = 2.0f;
	_entities.push_back(ent);
	ent.GetTransform()->translation.x = 4.0f;
	_entities.push_back(ent);

	return true;
}

bool Scene::Update()
{
	return true;
}

bool Scene::RenderUpdate()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	rlImGuiBegin();
	BeginMode3D(_camera);

	ImGui::Begin("ImGui");
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(200.0f, _height));

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

	for (Entity ent : _entities) {
		Transform *transform = ent.GetTransform();
		for (std::string guid : ent.GetGUIDs()) {
			if (ResourceManager::Instance().GetGUIDType(guid) == "Mesh") {
				MeshResource *mesh = (MeshResource *)ResourceManager::Instance().Load(guid);
				DrawModel(mesh->GetModel(), transform->translation, transform->scale.x, RED);
			}
		}
	}

	DrawGrid(20, 1.0f);

	EndMode3D();
	ImGui::End();
	rlImGuiEnd();
	EndDrawing();

	return true;
}
