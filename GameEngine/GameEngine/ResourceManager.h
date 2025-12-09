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
	bool _limitMemory = false;
	uint64_t _memoryLimit;
	uint64_t _memoryUsed;

	std::vector<std::string> _newPackage;
	std::thread workerThread[1];
	std::unordered_map<std::string, AssetData> _threadData;

	
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

	bool LoadResource(std::string guid, Resource *&resource);
	bool UnloadResource(std::string guid);
	bool LoadObject(Resource* &resource);

	void WorkerThread();
	int GetThreadDataSize();

	PackageManager *GetPackageManager();

	// Enables and sets memory limit (in bytes)
	void EnableMemoryLimit(uint64_t memoryLimit);
	// Disables memory limit
	void DisableMemoryLimit();
	// Gets memory limit (in bytes)
	uint64_t GetMemoryLimit();
	// Sets memory limit (in bytes)
	void SetMemoryLimit(uint64_t limit);

	// Gets memory used (in bytes)
	uint64_t GetMemoryUsed();
};