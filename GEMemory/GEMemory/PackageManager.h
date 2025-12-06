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

struct PackageEntry {
	uint64_t offset;
	uint64_t size;
	uint64_t sizeCompressed;
};

struct TOCEntry {
	std::string key;
	PackageEntry packageEntry;
};

struct MountedPackage {
	std::ifstream openFile; // Package file (open while mounted)
	std::unordered_map<std::string, PackageEntry> tocByPath; // Key (string): name of the file inside the package (file.extension)
	std::unordered_map<std::string, PackageEntry> tocByGuid; // Key (string): GUID of the asset found in its .meta file (file.extension.meta)
};

struct AssetData {
	std::unique_ptr<char[]> data;
	uint64_t size;
};

class PackageManager {
private:
	std::unordered_map<std::string, MountedPackage> _mountedPackages; // key (string): name of the package file (package.gepak)
	std::vector<std::string> _mountOrder; // The order of which an asset loading functions checks packages for assets (back = highest priority)

	// Loads asset from a specified mounted package
	bool LoadAsset(MountedPackage& mountedPackage, const PackageEntry& packageEntry, AssetData& asset);

public:
	PackageManager() = default;
	~PackageManager() = default;

	bool Pack(const std::string& source, const std::string& target);
	bool Unpack(const std::string& source, const std::string& target);

	// Mounts the package at the path source
	bool MountPackage(const std::string& source);
	// Unmounts the package mounted most recently
	bool UnmountPackage();
	// Unmount the package specifed by packageKey
	bool UnmountPackage(const std::string& packageKey);
};