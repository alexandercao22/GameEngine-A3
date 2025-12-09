#include "ResourceManager.h"

#include "GuidUtils.h"

ResourceManager::~ResourceManager() {
	for (auto pair : _cachedResources) {
		delete pair.second;
	}
}

bool ResourceManager::LoadResource(std::string guid, Resource *&resource) {
	if (guid.length() != GUID_STR_LENGTH) {
		std::cerr << "ResourceManager::LoadResource(): GUID is invalid" << std::endl;
		return false;
	}

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
		_cachedResources[guid]->Unload();
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
