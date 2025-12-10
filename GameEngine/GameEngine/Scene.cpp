#include "Scene.h"

#include "imgui.h"
#include "rlImGui.h"

#include "WinFileDialog.h"

#include "ResourceManager.h"
#include "MeshResource.h"
#include "TextureResource.h"

//#define DEBUG

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

		}
	}

	static char buf[64] = "";
	ImGui::InputText("File", buf, IM_ARRAYSIZE(buf));
	if (ImGui::Button("Load")) {
		std::string path = "Resource/" + std::string(buf);
	}
	ImGui::Separator();
	ImGui::Text("Camera Position:");
	ImGui::BulletText("X: %.2f", _camera.position.x);
	ImGui::BulletText("Y: %.2f", _camera.position.y);
	ImGui::BulletText("Z: %.2f", _camera.position.z);
	ImGui::Separator();

	std::vector<std::string> resources = ResourceManager::Instance().GetCachedResources();
	for (int i = 0; i < resources.size(); i++) {
		ImGui::Text("%s", resources[i].c_str());

	}
	
	ImGui::End();
	return true;
}

Scene::~Scene()
{
	for (Entity *ent : _entities) {
		delete ent;
	}

	for (ScenePart* part : _parts) {
		delete part;
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

	Mesh floorMesh = GenMeshPlane(40, 40, 1, 1);
	_floor = LoadModelFromMesh(floorMesh);


	
		//Initialize the parts
		ScenePart* part2 = new ScenePart;
		part2->Init({-40,0,0}, "Resources/niva1.gepak");
		_parts.push_back(part2);

		ScenePart* part3 = new ScenePart;
		part3->Init({0,0,-40}, "Resources/Mesh.gepak");
		_parts.push_back(part3);

		ScenePart* part4 = new ScenePart;
		part4->Init({-40,0,-40}, "Resources/Mesh.gepak");
		_parts.push_back(part4);

	std::string packagePath = "Resources/Mesh.gepak";
	if (!ResourceManager::Instance().GetPackageManager()->MountPackage(packagePath)) {
		std::cerr << "Scene::Init(): Could not load package: " << packagePath << std::endl;
		return false;
	}

	// Initialize camera
	_camera.position = { 0.0f, 2.0f, 10.0f };
	_camera.target = { 0.0f, 0.0f, 0.0f };
	_camera.up = { 0.0f, 1.0f, 0.0f };
	_camera.fovy = 90.0f;
	_camera.projection = CAMERA_PERSPECTIVE;

	EntityEnemy *ent = new EntityEnemy;
	ent->Init();
	ent->GetTransform()->translation.x = -40.0f;
	_entities.push_back(ent);

	ent = new EntityEnemy;
	ent->Init();
	ent->GetTransform()->translation.z = -40.0f;
	_entities.push_back(ent);

	ent = new EntityEnemy;
	ent->Init();
	ent->GetTransform()->translation.x = -40.0f;
	ent->GetTransform()->translation.z = -40.0f;
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

	static bool mouseLocked = false;
	if (IsKeyPressed(KEY_V))
	{
		mouseLocked = !mouseLocked;

		if (mouseLocked)
			DisableCursor();
		else
			EnableCursor();
	}
	int size = ResourceManager::Instance().GetThreadDataSize();
#ifdef DEBUG
	std::cout << size << std::endl;
#endif
	if ( size > 0) {
		// Load the model or texture from the data in ThreadData datastructure
		// All this complexity would not be necessary if we did not use Raylib
		Resource* res = new MeshResource;
		ResourceManager::Instance().LoadObject(res);
	}

	for (auto& part : _parts) {
		

		if (part->CheckDistance(_camera.position)) {
			if (!part->IsLoaded()) {
				// Load
				std::string path = part->GetPath();
				ResourceManager::Instance().AddPackage(path);

			}
		}
	}

	UpdateCamera(&_camera, CAMERA_FREE);
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
	Color colors[4] = { GREEN, DARKGREEN, LIME, BROWN };

	int j = 0;
	for (int i = 0; i < 2; i++) {
		for (int k = 0; k < 2; k++) {
			DrawModel(_floor, Vector3{ -40.0f + k * 40, 0, -40.0f + i * 40 }, 1.0f, colors[j]);
			j++;
		}
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

	DrawGrid(40, 1.0f);

	EndMode3D();
	rlImGuiEnd();
	EndDrawing();

	return true;
}
