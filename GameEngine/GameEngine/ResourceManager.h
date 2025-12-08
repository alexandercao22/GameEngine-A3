#pragma once
#include <unordered_map>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <mutex>
#include "Resource.h"
#include "MeshResource.h"


struct LoadState {
	bool loading;
	Resource* resource;
	std::condition_variable cv;
};

class ResourceManager 
{
private:
	// List of GUIDS
	std::unordered_map<std::string, std::string> _GUIDtoPath;
	std::unordered_map<std::string, std::string> _PathtoGUID;
	std::unordered_map<std::string, std::string> _GUIDtoType;
	// List of cached Resources
	//std::unordered_map<std::string, Resource*> _cachedResources;
	std::unordered_map<std::string, LoadState> _cachedResources;

	Resource* LoadFromDisk(std::string path, std::unique_lock<std::mutex> &lock);
	std::mutex _mutex;

public:
	// Singleton instance
	static ResourceManager &Instance() {
		static ResourceManager instance;
		return instance;
	}
	// Copy prevention
	ResourceManager(const ResourceManager &) = delete;
	ResourceManager &operator=(const ResourceManager &) = delete;

	ResourceManager() = default;
	~ResourceManager();

	bool Init();

	Resource* Load(std::string guid);
	bool Unload(std::string guid);

	std::string GetGUIDType(std::string guid);

	std::string SaveGUID(std::string path);
};