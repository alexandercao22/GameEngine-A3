#include "ResourceManager.h"

#include "MeshResource.h"

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

std::string ResourceManager::SaveGUID(std::string path) {
	// Checking if asset already exists in folder
	// to prohibit duplication

	std::cout << "Hejsan" << std::endl;
	for (auto paths : _PathtoGUID) {
		if (paths.first == path) {
			std::cout << "ERROR: Asset already exists in Resources." << std::endl;
			return {};
		}
	}
	

	std::string guid = GenerateGUID();

	_PathtoGUID.insert({ path, guid });
	_GUIDtoPath.insert({ guid, path });

	std::ofstream file("Resources/Assets.txt", std::ios::app);

	if (!file.is_open()) {
		std::cerr << "Failed to open Assets.txt for appending\n";
		return 0;
	}

	file << guid << ":" << "type" << path << "\n";

	return guid;

}

ResourceManager::~ResourceManager() {
	for (auto res : CachedResources) {
		delete res.second;
	}
}

void ResourceManager::Init() {
	std::ifstream file("Resources/Assets.txt");

	if (!file.is_open()) {
		std::cerr << "Failed to open Assets file" << std::endl;
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

}

Resource* ResourceManager::Load(std::string guid) {
	for (auto guids : CachedResources) {
		if (guids.first == guid) {
			CachedResources[guid]->RefAdd();
			return CachedResources[guid];
		}
	}
	int exist = 0;
	for (auto guids : _GUIDtoPath) {
		if (guids.first == guid) {
			return LoadFromDisk(_GUIDtoPath[guid]);
		}
	}
	// Save GUID
}

Resource* ResourceManager::LoadFromDisk(std::string path) {
	std::string guid = _PathtoGUID[path];
	std::string type = _GUIDtoType[guid];

	Resource* res = nullptr;
	if (type == "Mesh") {
		res = new MeshResource();
	}
	res->LoadFromDisk(path);
	CachedResources.insert({ guid, res });

	std::cout << "Hur många gånger anropas denna funktion?" << std::endl;

	return res;
	
}

bool ResourceManager::UnLoad(std::string GUID) {
	Resource* res = CachedResources[GUID];
	if (res == nullptr) {
		return false;
	}
	int ref = res->GetRef();
	if (ref == 1) {
		CachedResources.erase(GUID);
		return true;
	}
	else if (ref > 1) {
		CachedResources[GUID]->RefSub();
		return true;
	}
	else {
		return false;
	}
}