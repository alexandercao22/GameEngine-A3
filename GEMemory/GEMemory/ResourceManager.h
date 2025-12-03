#pragma once
#include <unordered_map>
#include <string>

class Resource {
private:
	int ReferenceCount = 0;
public:
	Resource* LoadFromDisk();
	void UnLoad();
	
};

class GUID {
public:
	std::string id;

	GUID() = default;
	GUID(const std::string& value) : id(value) {}

	bool operator== (const GUID& other) const {
		return id == other.id;
	}
};

class ResourceManger 
{
private:
	// List of GUIDS	
	std::unordered_map<GUID, std::string> GUIDtoPath;
	// List of cached Resources
	std::unordered_map<GUID, Resource*> CachedResources;

public:
	void Init();

	Resource* Load(GUID id);
	Resource* LoadFromDisk(std::string path);
	bool Unload(GUID id);
};