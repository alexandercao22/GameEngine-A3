#include "ResourceManager.h"

//Resource *ResourceManager::LoadFromDisk(std::string path) {
//	std::string guid = _PathtoGUID[path];
//	std::string type = _GUIDtoType[guid];
//
//	Resource *res = nullptr;
//	if (type == "Mesh") {
//		res = new MeshResource();
//	}
//	else if (type == "Texture") {
//		res = new TextureResource();
//	}
//	res->LoadFromDisk(path);
//	_cachedResources.insert({ guid, res });
//
//	return res;
//}

ResourceManager::~ResourceManager() {
	for (auto pair : _cachedResources) {
		delete pair.second;
	}
}

bool ResourceManager::LoadResource(std::string guid, Resource *&resource) {
	auto pair = _cachedResources.find(guid);
	if (pair != _cachedResources.end()) {
		// Resource already exists in cache
		resource = pair->second;
		pair->second->RefAdd();
		return true;
	}
	else {
		// Resource does not exist in cache -> load it from mounted package
		AssetData data;
		if (!_packageManager.LoadAssetByGuid(guid, data)) {
			std::cerr << "ResourceManager::LoadResource(): Could not load resource" << std::endl;
			return false;
		}
		resource->SetData(data);

		resource->RefAdd();
		_cachedResources.emplace(guid, resource);
		return true;
	}
}

bool ResourceManager::UnloadResource(std::string guid) {
	Resource* res = _cachedResources[guid];
	if (res == nullptr) {
		std::cerr << "ResourceManager::UnloadResource(): Resource with GUID is nullptr" << std::endl;
		return false;
	}
	int ref = res->GetRef();
	if (ref == 1) {
		_cachedResources[guid]->UnLoad();
		_cachedResources.erase(guid);
		return true;
	}
	else if (ref > 1) {
		_cachedResources[guid]->RefSub();
		return true;
	}
	else {
		std::cerr << "ResourceManager::UnloadResource(): Failed unloading resource with GUID: " << guid << std::endl;
		return false;
	}
}

PackageManager *ResourceManager::GetPackageManager()
{
	return &_packageManager;
}

//std::string ResourceManager::GetGUIDType(std::string guid)
//{
//	return _GUIDtoType[guid];
//}
//
//std::string ResourceManager::SaveGUID(std::string path) {
//	// Checking if asset already exists in folder to prohibit duplication
//	for (auto paths : _PathtoGUID) {
//		if (paths.first == path) {
//			std::cerr << "ResourceManager::SaveGUID(): Asset already exists in Resources" << std::endl;
//			return "";
//		}
//	}
//
//	std::string guid = GenerateGUID();
//
//	_PathtoGUID.insert({ path, guid });
//	_GUIDtoPath.insert({ guid, path });
//
//	std::ofstream file("Resources/Assets.txt", std::ios::app);
//	if (!file.is_open()) {
//		std::cerr << "ResourceManager::SaveGUID(): Failed to open Assets.txt for appending" << std::endl;
//		return "";
//	}
//
//	std::string type = "";
//	if (path.find(".obj") != std::string::npos ||
//		path.find(".gltf") != std::string::npos) {
//		type = "Mesh";
//	}
//	else if (path.find(".png") != std::string::npos ||
//		path.find(".jpg") != std::string::npos) {
//		type = "Texture";
//	}
//
//	file << guid << ":" << type << ":" << path << "\n";
//	return guid;
//}
