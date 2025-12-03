#pragma once

#include "Libraries/lz4/lz4.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

inline constexpr char SIGNATURE[6] = "GEPAK"; // Signature for files packaged by this package manager

struct PackageHeader {
	char signature[5];
	uint32_t AssetCount;
	uint64_t tableOfContentsOffset;
};

struct TOCEntry {
	std::string key;
	PackageEntry entryData;
};

struct PackageEntry {
	uint64_t offset;
	uint64_t size;
	uint64_t sizeCompressed;
};

struct MountedPackage {
	std::ifstream openFile;
	std::unordered_map<std::string, PackageEntry> tableOfContents; // Key (string): name of the file inside the package
};

struct AssetData {
	std::unique_ptr<char[]> data;
	uint64_t size;
};

class PackageManager {
private:
	std::vector<MountedPackage> mountedPackages;

public:
	PackageManager() = default;
	~PackageManager();

	bool Pack(const std::string& path);
	bool Unpack(const std::string& path);

	bool MountPackage(const std::string& path);
	AssetData LoadAsset(const std::string& assetName);
};