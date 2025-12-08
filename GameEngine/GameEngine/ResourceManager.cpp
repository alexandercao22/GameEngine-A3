#include "ResourceManager.h"

#include "MeshResource.h"
#include "TextureResource.h"
//#include <mutex>

std::string GenerateGUID() {
	static std::random_device rd;
	static std::mt19937_64 gen(rd());
	static std::uniform_int_distribution<uint64_t> dis;

	uint64_t part1 = dis(gen);
	uint64_t part2 = dis(gen);

	uint8_t uuid[16];
	for (int i = 0; i < 8; i++) {
		uuid[i] = (part1 >> (i * 8)) & 0xFF;
	}
	for (int i = 0; i < 8; i++) {
		uuid[8 + i] = (part2 >> (i * 8)) & 0xFF;
	}

	uuid[6] = (uuid[6] & 0x0F) | 0x40;

	uuid[8] = (uuid[8] & 0x3F) | 0x80;

	std::stringstream ss;

	ss << std::hex << std::setfill('0');

	for (int i = 0; i < 16; i++) {
		ss << std::setw(2) << (int)uuid[i];
		if (i == 3 || i == 5 || i == 7 || i == 9) {
			ss << "-";
		}
	}

	return ss.str();
}

Resource *ResourceManager::LoadFromDisk(std::string path, std::unique_lock<std::mutex> &lock) {
	//lock.unlock();
	std::string guid = _PathtoGUID[path];
	std::string type = _GUIDtoType[guid];

	Resource *res = nullptr;
	if (type == "Mesh") {
		res = new MeshResource();
	}
	else if (type == "Texture") {
		res = new TextureResource();
	}
	res->LoadFromDisk(path);
	//// Only one thread is allowed to write to CachedResources.
	//std::lock_guard<std::mutex> lock(_mutex);
	//LoadState state;
	//state.resource = res;
	//state.loading = false;
	//_cachedResources.insert({ guid, res });

	return res;
}

ResourceManager::~ResourceManager() {
	for (auto& res : _cachedResources) {
		delete res.second.resource;
	}
}

bool ResourceManager::Init() {
	std::ifstream file("Resources/Assets.txt");

	if (!file.is_open()) {
		std::cerr << "ResourceManager::Init(): Failed to open Assets file" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		// hardcoded substrings since every GUID is 35 spots long
		// and the colon is on the 36th spot
		std::string guid = line.substr(0, 36);
		int secondColon = line.find_last_of(':');
		int typeSize = secondColon - 37;
		std::string type = line.substr(37, typeSize);
		std::string path = line.substr(secondColon + 1);

		_PathtoGUID.insert({ path, guid });
		_GUIDtoPath.insert({ guid, path}); 
		_GUIDtoType.insert({ guid, type}); 
	}
	return true;
}

Resource *ResourceManager::Load(std::string guid) {
	
	/*{
		for (auto guids : _cachedResources) {
			if (guids.first == guid) {
				std::lock_guard<std::mutex> lock(_mutex);
				_cachedResources[guid]->RefAdd();
				return _cachedResources[guid];
			}
		}
	}
	
	for (auto guids : _GUIDtoPath) {
		if (guids.first == guid) {
			{
				std::lock_guard<std::mutex> lock(_mutex);
				LoadState state;
				state.loading = true;
			}
			return LoadFromDisk(_GUIDtoPath[guid]);
		}
	}*/
	// Save GUID


	std::unique_lock<std::mutex> lock(_mutex);

	auto& state = _cachedResources[guid];

	if (state.resource) {
		state.resource->RefAdd();
		return state.resource;
	}

	if (state.loading) {
		state.cv.wait(lock, [&] {return state.resource != nullptr; });
		state.resource->RefAdd();
		return state.resource;
	}
	
	state.loading = true;
	lock.unlock();

	Resource* res = LoadFromDisk(_GUIDtoPath[guid], lock);

	lock.lock();
	state.resource = res;
	state.loading = false;
	res->RefAdd();
	state.cv.notify_all();
	return res;
}

bool ResourceManager::Unload(std::string guid) {
	std::lock_guard<std::mutex> lock(_mutex);
	auto& res = _cachedResources[guid];
	if (res.resource == nullptr) {
		return false;
	}
	int ref = res.resource->GetRef();
	if (ref == 1) {
		//_cachedResources[guid]->UnLoad();
		res.resource->UnLoad();
		_cachedResources.erase(guid);
		return true;
	}
	else if (ref > 1) {
		//_cachedResources[guid]->RefSub();
		res.resource->RefSub();
		return true;
	}
	else {
		std::cerr << "ResourceManager::Unload(): Asset already unloaded " << std::endl;
		return false;
	}
}

std::string ResourceManager::GetGUIDType(std::string guid)
{
	return _GUIDtoType[guid];
}

std::string ResourceManager::SaveGUID(std::string path) {
	// Checking if asset already exists in folder to prohibit duplication
	for (auto paths : _PathtoGUID) {
		if (paths.first == path) {
			std::cerr << "ResourceManager::SaveGUID(): Asset already exists in Resources" << std::endl;
			return "";
		}
	}

	std::string guid = GenerateGUID();

	_PathtoGUID.insert({ path, guid });
	_GUIDtoPath.insert({ guid, path });

	std::ofstream file("Resources/Assets.txt", std::ios::app);
	if (!file.is_open()) {
		std::cerr << "ResourceManager::SaveGUID(): Failed to open Assets.txt for appending" << std::endl;
		return "";
	}

	std::string type = "";
	if (path.find(".obj") != std::string::npos ||
		path.find(".gltf") != std::string::npos) {
		type = "Mesh";
	}
	else if (path.find(".png") != std::string::npos ||
		path.find(".jpg") != std::string::npos) {
		type = "Texture";
	}

	file << guid << ":" << type << ":" << path << "\n";
	return guid;
}
