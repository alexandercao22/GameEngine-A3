#include "ResourceManager.h"
ResourceManager::ResourceManager() {
	workerThread.emplace_back(&ResourceManager::WorkerThread, this);
}

ResourceManager::~ResourceManager() {
	workerThread.at(0).join();
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
			std::cerr << "ResourceManager::LoadResource(): Could not load resource data from package" << std::endl;
			return false;
		}

		if (!resource->LoadFromData(data.data.get(), data.size)) {
			std::cerr << "ResourceManager::LoadResource(): Could not load resource from raw data" << std::endl;
			return false;
		}

		resource->RefAdd();
		_cachedResources.emplace(guid, resource);
		_memoryUsed += resource->GetMemoryUsage();

		// Memory limit warning
		if (_limitMemory && (_memoryUsed >= _memoryLimit)) {
			std::cerr << "Warning: ResourceManager memory limit reached: " << _memoryUsed << " of " << _memoryLimit << " bytes used" << std::endl;
		}

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
		// References goes to 0 -> remove resource from cache
		_memoryUsed -= _cachedResources[guid]->GetMemoryUsage();
		_cachedResources[guid]->UnLoad();
		_cachedResources.erase(guid);
		return true;
	}
	else if (ref > 1) {
		// References does not go to zero -> subtract from reference count
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

void ResourceManager::EnableMemoryLimit(uint64_t limit)
{
	_limitMemory = true;
	_memoryLimit = limit;
}

void ResourceManager::DisableMemoryLimit()
{
	_limitMemory = false;
}

uint64_t ResourceManager::GetMemoryLimit()
{
	return _memoryLimit;
}

void ResourceManager::SetMemoryLimit(uint64_t limit)
{
	_memoryLimit = limit;
}

uint64_t ResourceManager::GetMemoryUsed()
{
	return _memoryUsed;
}

bool ResourceManager::AddPackage(std::string path) {
	std::lock_guard<std::mutex> lock(_packageMutex);
	_newPackage.push_back(path);

	return true;
}

void ResourceManager::WorkerThread() {
	// This thread will run in
	while (true) {
		std::string package;
		{
			std::unique_lock<std::mutex> lock(_packageMutex);
			if (_newPackage.empty())
				continue;
			package = std::move(_newPackage.front());
			_newPackage.erase(_newPackage.begin());
		}
		//if (!_newPackage.empty()) {

			if (!_packageManager.MountPackage(package)) {
				std::cerr << "ResourceManager::MountPackage(): Could not mount package" << std::endl;
			}
			//MountedPackage pack = _packageManager.GetMountedPackage();
			
			std::vector<std::string> guids = _packageManager.GetGUIDsInLastMountedPackage();

			//std::string guid = pack.tocByGuid[];
			for (const std::string& guid : guids) {

				AssetData data;
				if (!_packageManager.LoadAssetByGuid(guid, data)) {
					std::cerr << "ResourceManager::LoadResource(): Could not load resource" << std::endl;

				}
				std::lock_guard<std::mutex> lock(_threadDataMutex);
				_threadData.emplace(guid, std::move(data));
				
			}
			//std::lock_guard<std::mutex> lock(_threadDataMutex);

			//_newPackage.erase(_newPackage.begin());
		//}
	}
}

int ResourceManager::GetThreadDataSize() {
	return _threadData.size();
}

bool ResourceManager::LoadObject(Resource* &resource) {
	//std::mutex _threadDataMutex;
	AssetData data;
	{

		std::lock_guard<std::mutex> lock(_threadDataMutex);
		if (_threadData.empty()) {
			std::cerr << "ResourceManager::LoadObject(): AssetData vector is empty" << std::endl;
			return false;
		}
		
		auto it = _threadData.begin();
		data = std::move(it->second);
		_threadData.erase(it);
	
	}
	std::string text(data.data.get(), data.size);
	resource->LoadFromData(text.data(), text.size());

	return true;
	
}

std::vector<std::string> ResourceManager::GetCachedResources() {
	std::vector<std::string> GUID;
	for (auto res : _cachedResources) {
		GUID.push_back(res.first);
	}
	return GUID;
}