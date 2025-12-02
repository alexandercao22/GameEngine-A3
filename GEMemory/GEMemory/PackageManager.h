#pragma once

#include "Libraries/lz4/lz4.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

struct PackageHeader {
	char signature[9]; // GEPACKAGE
	unsigned int AssetCount;
	unsigned int tableOfContentsOffset;
};

struct PackageEntry {
	unsigned int offset;
	unsigned int size;
	unsigned int sizeCompressed;
};

struct MountedPackage {
	std::ifstream openFile;
	std::unordered_map<std::string, PackageEntry> tableOfContents; // Key (string): name of the file inside the package
};

struct AssetData {
	std::unique_ptr<char[]> data;
	unsigned int size;
};

class PackageManager {
private:
	std::vector<MountedPackage> mountedPackages;

public:
	PackageManager() = default;
	~PackageManager();

	bool MountPackage(const std::string& path);
	AssetData LoadAsset(const std::string& assetName);
};