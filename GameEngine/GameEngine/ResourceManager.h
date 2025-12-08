#pragma once
#include <unordered_map>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "Resource.h"
#include "PackageManager.h"

class ResourceManager 
{
private:
	//// List of GUIDS
	//std::unordered_map<std::string, std::string> _GUIDtoPath;
	//std::unordered_map<std::string, std::string> _PathtoGUID;
	//std::unordered_map<std::string, std::string> _GUIDtoType;
	//Resource* LoadFromDisk(std::string path);
	
	PackageManager _packageManager; // Manages packages and resource loading from packages
	std::unordered_map<std::string, Resource*> _cachedResources; // Currently used resources with reference counts

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

	bool LoadResource(std::string guid, Resource* resource);
	bool UnloadResource(std::string guid);

	//std::string GetGUIDType(std::string guid);
	//std::string SaveGUID(std::string path);
};