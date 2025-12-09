#include "ResourceManager.h"

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
		_memoryUsed += sizeof(_cachedResources[guid]);

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
		_memoryUsed -= sizeof(_cachedResources[guid]);
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

void ResourceManager::WorkerThread() {
	// This thread will run in
	while (true) {
		if (!_newPackage.empty()) {

			if (!_packageManager.MountPackage(_newPackage.at(0))) {
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
				_threadData.emplace(guid, std::move(data));
				
			}

			_newPackage.at(0).erase();
		}
	}
}

int ResourceManager::GetThreadDataSize() {
	return _threadData.size();
}

bool ResourceManager::LoadObject(Resource* &resource) {
	
	//resource->LoadFromData(_threadData.at(0).data, _threadData.at(0).size);
	
}