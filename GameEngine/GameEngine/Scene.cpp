#include "Scene.h"
#include "Settings.h"

#include "imgui.h"
#include "rlImGui.h"

#include "WinFileDialog.h"
#include <chrono>

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

	std::string meshesPkg = "Resources/Meshes.gepak";
	ResourceManager::Instance().GetPackageManager()->Pack("Resources/Meshes", "Resources");
	if (!ResourceManager::Instance().GetPackageManager()->MountPackage(meshesPkg)) {
		std::cerr << "Scene::Init(): Could not load package: " << meshesPkg << std::endl;
		return false;
	}

	std::string texturesPkg = "Resources/Textures.gepak";
	ResourceManager::Instance().GetPackageManager()->Pack("Resources/Textures", "Resources");
	if (!ResourceManager::Instance().GetPackageManager()->MountPackage(texturesPkg)) {
		std::cerr << "Scene::Init(): Could not load package: " << texturesPkg << std::endl;
		return false;
	}

	// Initialize camera
	_camera.position = { 0.0f, 5.0f, 10.0f };
	_camera.target = { 0.0f, 0.0f, 0.0f };
	_camera.up = { 0.0f, 1.0f, 0.0f };
	_camera.fovy = 90.0f;
	_camera.projection = CAMERA_PERSPECTIVE;

	const int numEnemies = 10000;
	const int numRow = 10;
	auto t0 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numEnemies; i++) {
		EntityEnemy *ent = new EntityEnemy;
		ent->Init();
		Transform *t = ent->GetTransform();
		t->translation.x = (int)(i / numRow) * -5;
		t->translation.z = (i % numRow) * -5;
		_entities.push_back(ent);
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = t1 - t0;

#ifdef DEBUG
	std::cout << "Time to load " << numEnemies << " EntityEnemy: " << duration.count() << "s" << std::endl;
#endif

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

	//if (!RenderInterface()) {
	//	return false;
	//}

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
