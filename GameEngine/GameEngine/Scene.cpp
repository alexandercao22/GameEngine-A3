#include "Scene.h"

#include "imgui.h"
#include "rlImGui.h"

#include "WinFileDialog.h"

#include "ResourceManager.h"
#include "MeshResource.h"
#include "TextureResource.h"

bool Scene::RenderInterface()
{
	ImGui::Begin("ImGui");
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(200.0f, _height));

	if (ImGui::Button("Open File Explorer")) {
		std::string path = OpenFileDialog();
		if (path.find(".obj") != std::string::npos ||
			path.find(".gltf") != std::string::npos ||
			path.find(".png") != std::string::npos ||
			path.find(".jpg") != std::string::npos) {
			CopyFileToResources(path, "Resources");

			size_t backslash = path.find_last_of("\\") + 1;
			std::string file = "Resources/" + path.substr(backslash, path.length() - backslash);
			std::cout << file << std::endl;

			ResourceManager::Instance().SaveGUID(file);
		}
	}

	static char buf[64] = "";
	ImGui::InputText("File", buf, IM_ARRAYSIZE(buf));
	if (ImGui::Button("Load")) {
		std::string path = "Resource/" + std::string(buf);
		ResourceManager::Instance().SaveGUID(path);
	}

	ImGui::End();
	return true;
}

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
	ent.AddGUID("69f74b0d-b87d-44b0-bb89-4fb94b5243c8"); // mushroom.obj
	ent.GetTransform()->translation.x = 0.0f;
	_entities.push_back(ent);
	ent.GetTransform()->translation.x = 2.0f;
	_entities.push_back(ent);
	ent.GetTransform()->translation.x = 4.0f;
	_entities.push_back(ent);

	ent = Entity();
	ent.AddGUID("90c94e91-dd63-441f-9372-b12a77b8036a"); // cubeWithTexture.obj
	ent.AddGUID("2fe16579-8532-47b5-a6d4-a36f22611c3c"); // cubemaps_skybox.png
	ent.GetTransform()->translation.y = 4.0f;
	_entities.push_back(ent);

	ent = Entity();
	//ent.AddGUID(ResourceManager::Instance().SaveGUID("Resources/Duck.gltf"));
	ent.AddGUID("27471003-7f27-42e3-a791-c159b33f5198");
	ent.GetTransform()->translation.x = 4.0f;
	ent.GetTransform()->translation.y = 4.0f;
	ent.GetTransform()->translation.z = 4.0f;
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

	if (!RenderInterface()) {
		return false;
	}

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
		MeshResource *mesh = nullptr;
		TextureResource *texture = nullptr;
		for (std::string guid : ent.GetGUIDs()) {
			if (ResourceManager::Instance().GetGUIDType(guid) == "Mesh") {
				mesh = (MeshResource *)ResourceManager::Instance().Load(guid);
			}
			else if (ResourceManager::Instance().GetGUIDType(guid) == "Texture") {
				texture = (TextureResource *)ResourceManager::Instance().Load(guid);
			}
		}

		if (texture != nullptr) {
			SetMaterialTexture(&mesh->GetModel().materials[0], MATERIAL_MAP_DIFFUSE, texture->GetTexture());
			Vector3 rotation = { transform->rotation.x, transform->rotation.y, transform->rotation.z };
			DrawModelEx(mesh->GetModel(), transform->translation, rotation, transform->rotation.w, transform->scale, WHITE);
		}
		else {
			Vector3 rotation = { transform->rotation.x, transform->rotation.y, transform->rotation.z };
			DrawModelEx(mesh->GetModel(), transform->translation, rotation, transform->rotation.w, transform->scale, RED);
		}
	}

	DrawGrid(20, 1.0f);

	EndMode3D();
	rlImGuiEnd();
	EndDrawing();

	return true;
}
