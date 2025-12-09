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

			//ResourceManager::Instance().SaveGUID(file);
		}
	}

	static char buf[64] = "";
	ImGui::InputText("File", buf, IM_ARRAYSIZE(buf));
	if (ImGui::Button("Load")) {
		std::string path = "Resource/" + std::string(buf);
		//ResourceManager::Instance().SaveGUID(path);
	}

	ImGui::End();
	return true;
}

Scene::~Scene()
{
	for (Entity *ent : _entities) {
		delete ent;
	}

	ResourceManager::Instance().GetPackageManager()->UnmountAllPackages();
}

bool Scene::Init(unsigned int width, unsigned int height)
{
	_width = width;
	_height = height;

	InitWindow(_width, _height, "Game Engine Assignment 3");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	/*
		Initialize the parts

		ScenePart part1;
		part1.Init({0,0,0}, "Resources/Mesh.gepak");
		_parts.push_back(part1);

		ScenePart part2;
		part1.Init({20,0,0}, "Resources/Mesh.gepak");
		_parts.push_back(part2);

		ScenePart part3;
		part1.Init({-20,0,0}, "Resources/Mesh.gepak");
		_parts.push_back(part3);

		ScenePart part4;
		part1.Init({20,0,20}, "Resources/Mesh.gepak");
		_parts.push_back(part4);

		ResourceManager::Instance().GetPackageManager()->MountPackage(part1.GetPath()) {
			std::cerr << blablabla << std::endl;
		}
	*/

	std::string packagePath = "Resources/Mesh.gepak";
	if (!ResourceManager::Instance().GetPackageManager()->MountPackage(packagePath)) {
		std::cerr << "Scene::Init(): Could not load package: " << packagePath << std::endl;
		return false;
	}

	// Initialize camera
	_camera.position = { 0.0f, 5.0f, 10.0f };
	_camera.target = { 0.0f, 0.0f, 0.0f };
	_camera.up = { 0.0f, 1.0f, 0.0f };
	_camera.fovy = 90.0f;
	_camera.projection = CAMERA_PERSPECTIVE;

	EntityEnemy *ent = new EntityEnemy;
	ent->Init();
	ent->GetTransform()->translation.x = 4.0f;
	_entities.push_back(ent);

	ent = new EntityEnemy;
	ent->Init();
	ent->GetTransform()->translation.y = 4.0f;
	_entities.push_back(ent);

	ent = new EntityEnemy;
	ent->Init();
	ent->GetTransform()->translation.x = 4.0f;
	ent->GetTransform()->translation.y = 4.0f;
	ent->GetTransform()->translation.z = 4.0f;
	_entities.push_back(ent);

	return true;
}

bool Scene::Update()
{
	/* 
		Check distance between parts
		if distance is appropiate add the work to the thread vector
		so it can see it has work to do.
		Add the data from the thread onto the the RM queue.

		Check if the RM queue is empty. If not, load the model/texture
		with the data. 


	*/

	if (ResourceManager::Instance().GetThreadDataSize() > 0) {
		// Load the model or texture from the data in ThreadData datastructure
		// All this complexity would not be necessary if we did not use Raylib
		// Resource* res = new MeshResource*;
		// ResourceManager::Instance().LoadObject(res);
	}
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

	for (Entity *ent : _entities) {
		Transform *transform = ent->GetTransform();
		MeshResource *mesh = ent->GetMesh();
		TextureResource *texture = ent->GetTexture();
		if (mesh != nullptr) {
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
	}

	DrawGrid(20, 1.0f);

	EndMode3D();
	rlImGuiEnd();
	EndDrawing();

	return true;
}
