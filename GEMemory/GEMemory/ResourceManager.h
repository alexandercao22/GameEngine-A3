#pragma once
#include <unordered_map>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "Resource.h"


class GUID {
public:
	std::string id;

	GUID() = default;
	GUID(const std::string& value) : id(value) {}

	bool operator== (const GUID& other) const {
		return id == other.id;
	}
};

class ResourceManager 
{
private:
	// List of GUIDS	
	std::unordered_map<std::string, std::string> _GUIDtoPath;
	std::unordered_map<std::string, std::string> _PathtoGUID;
	std::unordered_map<std::string, std::string> _GUIDtoType;
	// List of cached Resources
	std::unordered_map<std::string, Resource*> _cachedResources;

	Resource* LoadFromDisk(std::string path);

public:
	ResourceManager() = default;
	~ResourceManager();

	bool Init();

	Resource* Load(std::string guid);
	bool Unload(std::string guid);

	std::string SaveGUID(std::string path);
};