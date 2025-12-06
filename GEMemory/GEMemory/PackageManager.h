#pragma once

#include "Libraries/lz4/lz4.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

inline constexpr char SIGNATURE[8] = "GEPAKV0"; // Signature for files packaged by this package manager

// Written at the very start of a package file
struct PackageHeader {
	char signature[5];
	uint32_t AssetCount;
	uint64_t tableOfContentsOffset;
};

// Specifies the details of a file inside a package
struct PackageEntry {
	uint64_t offset;
	uint64_t size;
	uint64_t sizeCompressed;
};

// Represents a file in a package
struct TOCEntry {
	std::string guid;
	std::string key;
	PackageEntry packageEntry;
};

// Supports lookup by GUID and by path
struct MountedPackage {
	std::ifstream openFile; // Package file (open while mounted)
	std::unordered_map<std::string, PackageEntry> tocByPath; // Key (string): name of the file inside the package (file.extension)
	std::unordered_map<std::string, PackageEntry> tocByGuid; // Key (string): GUID of the asset found in its .meta file (file.extension.meta)
};

// Used to load and store asset file data
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

	// Creates a package from a directory specified by source (path) inside directory specified by target (path)
	bool Pack(const std::string& source, const std::string& target);
	// Creates a directory from a package specifed by source (path) inside directory specifed by target (path)
	bool Unpack(const std::string& source, const std::string& target);

	// Mounts the package at the path source
	bool MountPackage(const std::string& source);
	// Unmounts the package mounted most recently
	bool UnmountPackage();
	// Unmount the package specifed by packageKey
	bool UnmountPackage(const std::string& packageKey);

	// Loads asset specified by GUID
	bool LoadAssetByGuid(const std::string& guid, AssetData& asset);
	// Loads asset specified by path relative to the package that holds it
	bool LoadAssetByPath(const std::string& path, AssetData& asset);
};