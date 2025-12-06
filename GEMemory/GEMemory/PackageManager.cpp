#include "PackageManager.h"
#include <filesystem>
#include "GuidUtils.h"
#include "Settings.h"

namespace fs = std::filesystem;

bool PackageManager::LoadAsset(MountedPackage& mountedPackage, const PackageEntry& packageEntry, AssetData& asset)
{
	// Read compressed data into a buffer
	AssetData compressedData;
	compressedData.size = packageEntry.sizeCompressed;
	compressedData.data = std::make_unique<char[]>(packageEntry.sizeCompressed);

	mountedPackage.openFile.clear(); // Resets read if EOF has been hit
	mountedPackage.openFile.seekg(packageEntry.offset);
	mountedPackage.openFile.read(compressedData.data.get(), compressedData.size);

	// Decompress data from buffer
	AssetData uncompressedData;
	uncompressedData.size = packageEntry.size;
	uncompressedData.data = std::make_unique<char[]>(packageEntry.size);

	int uncompressedSize = LZ4_decompress_safe(
		compressedData.data.get(),
		uncompressedData.data.get(),
		static_cast<int>(compressedData.size),
		static_cast<int>(uncompressedData.size)
	);

	if (uncompressedSize < 0) {
		std::cerr << "PackageManager::LoadAsset(): Decompression failed" << std::endl;
		return false;
	}

	asset = std::move(uncompressedData);

	return true;
}

bool PackageManager::Pack(const std::string& source, const std::string& target)
{
	fs::path sourcePath(source);
	fs::path targetPath(target);

	// Path validity checks
	if (!fs::is_directory(sourcePath)) {
		std::cerr << "PackageManager::Pack(): Source is not a directory" << std::endl;
		return false;
	}

	if (!fs::is_directory(targetPath)) {
		std::cerr << "PackageManager::Pack(): Target is not a directory" << std::endl;
		return false;
	}

	// Output file (package)
	std::string packageName = sourcePath.stem().generic_string() + ".gepak";
	targetPath = targetPath / packageName;
	std::ofstream out(targetPath, std::ios::binary);
	if (!out) { 
		std::cerr << "PackageManager::Pack(): Unable to create output file" << std::endl;
		return false;
	}

	// Offset writing position to account for header (To be written at the front of the package)
	PackageHeader header = {};
	out.seekp(sizeof(PackageHeader));

	// Table of contents (To be written at the back of the package)
	std::vector<TOCEntry> toc;


	if (DEBUG) {
		std::cout << "Starting packing: " << packageName << std::endl;
	}

	// Packaging directory
	for (const auto& dirEntry : fs::recursive_directory_iterator(sourcePath)) {
		if (fs::is_regular_file(dirEntry)) {
			fs::path entryPath = dirEntry.path();

			// .meta files are not to be packed as guid will be stored in the toc
			if (entryPath.extension() == ".meta") {
				continue;
			}

			fs::path relativePath = fs::relative(entryPath, sourcePath); // Relative path for TOC entry
			std::string key = relativePath.generic_string();

			// Read file
			std::ifstream in(entryPath, std::ios::binary);
			if (!in) {
				std::cerr << "PackageManager::Pack(): Could not read file" << std::endl;
				return false;
			}

			AssetData uncompressedData;
			in.seekg(0, std::ios::end); // Set cursor to end of file
			uncompressedData.size = static_cast<uint64_t>(in.tellg()); // Get cursor pos
			in.seekg(0, std::ios::beg); // Reset curosr to start of file
			uncompressedData.data = std::make_unique<char[]>(uncompressedData.size);
			in.read(uncompressedData.data.get(), uncompressedData.size); // Filling AssetData with file contents

			AssetData compressedData;
			int maxSizeCompressed = LZ4_compressBound(uncompressedData.size); // Maximum size the compressed version can reach
			compressedData.data = std::make_unique<char[]>(maxSizeCompressed);

			// Compressing file
			int sizeCompressed = LZ4_compress_default(uncompressedData.data.get(), compressedData.data.get(), uncompressedData.size, maxSizeCompressed);
			if (sizeCompressed == 0) {
				std::cerr << "PackageManager::Pack(): Compression error" << std::endl;
				return false;
			}
			compressedData.size = sizeCompressed; // Correct the size of the data buffer

			TOCEntry entry;
			if (!GuidUtils::GetOrGenerateGuid(entryPath, entry.guid)) {
				std::cerr << "PackageManager::Pack(): Could not get or generate GUID for " << entryPath << std::endl;
			}
			entry.key = key;
			entry.packageEntry.offset = static_cast<uint64_t>(out.tellp());
			entry.packageEntry.size = uncompressedData.size;
			entry.packageEntry.sizeCompressed = static_cast<uint64_t>(sizeCompressed);

			toc.push_back(entry);
			
			// Write the compressed data to the output file
			out.write(compressedData.data.get(), compressedData.size);

			if (DEBUG) {
				std::cout << "Packed " << key << " (" << uncompressedData.size << " -> " << compressedData.size << " bytes)" << std::endl;
			}
		}
	}

	// Finalizing header
	header.tableOfContentsOffset = out.tellp(); // Recording starting offset for the TOC
	header.AssetCount = toc.size();
	std::memcpy(header.signature, SIGNATURE, sizeof(header.signature));

	// Writing table of contents to the back of the package
	for (TOCEntry& entry : toc) {
		// GUID
		out.write(entry.guid.data(), GUID_STR_LENGTH);

		// The length of the key
		uint32_t keyLength = static_cast<uint32_t>(entry.key.size());
		out.write(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

		// The key
		out.write(entry.key.data(), keyLength);

		// The entry data (PackageEntry)
		out.write(reinterpret_cast<char*>(&entry.packageEntry), sizeof(entry.packageEntry));
	}

	out.seekp(0);
	out.write(reinterpret_cast<char*>(&header), sizeof(header));

	if (DEBUG) {
		std::cout << "Finished packing: " << packageName << std::endl;
	}

	return true;
}

bool PackageManager::Unpack(const std::string& source, const std::string& target)
{
	fs::path sourcePath(source);
	fs::path targetPath(target); // Should this be an input parameter?

	// Path validity checks
	if (!fs::is_regular_file(sourcePath)) {
		std::cerr << "PackageManager::Unpack(): Source is not a file" << std::endl;
		return false;
	}

	if (!fs::is_directory(targetPath)) {
		std::cerr << "PackageManager::Unpack(): Target is not a directory" << std::endl;
		return false;
	}

	// Read file
	std::ifstream in(sourcePath, std::ios::binary);
	if (!in) {
		std::cerr << "PackageManager::Unpack(): Could not read file" << std::endl;
		return false;
	}

	PackageHeader header;
	in.read(reinterpret_cast<char*>(&header), sizeof(header));
	
	// Signature check
	if (std::memcmp(header.signature, SIGNATURE, sizeof(header.signature)) != 0) {
		std::cerr << "PackageManager::Unpack(): Signature does not match" << std::endl;
		return false;
	}

	// Loop through toc and collect all entries
	in.seekg(header.tableOfContentsOffset);
	std::vector<TOCEntry> toc;
	for (int i = 0; i < header.AssetCount; i++) {
		TOCEntry entry;

		// The GUID
		std::string guid;
		guid.resize(GUID_STR_LENGTH);
		in.read(entry.guid.data(), GUID_STR_LENGTH);

		// The length of the key
		uint32_t keyLength;
		in.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

		// The key
		std::string key;
		entry.key.resize(keyLength);
		in.read(entry.key.data(), keyLength);

		// The entry data (PackageEntry)
		in.read(reinterpret_cast<char*>(&entry.packageEntry), sizeof(entry.packageEntry));
		
		toc.push_back(entry);
	}
	in.clear(); // In case EOF is hit

	// Create the unpacked package directory
	targetPath = targetPath / sourcePath.stem();
	if (fs::exists(targetPath)) {
		std::cerr << "PackageManager::Unpack(): Target already contains a directory with package name" << std::endl;
		return false;
	}
	fs::create_directory(targetPath);

	if (DEBUG) {
		std::cout << "Starting unpacking: " << sourcePath.filename().string() << std::endl;
	}

	// Go through all entries and restore the files
	for (auto& entry : toc) {
		fs::path relativePath(entry.key);
		fs::path filePath = targetPath / relativePath;

		// Creating necessary directories within the package
		if (filePath.has_parent_path()) {
			fs::create_directories(filePath.parent_path());
		}
		
		// Reading the compressed data from the package
		AssetData compressedData;
		compressedData.size = entry.packageEntry.sizeCompressed;
		compressedData.data = std::make_unique<char[]>(entry.packageEntry.sizeCompressed);

		in.seekg(entry.packageEntry.offset);
		in.read(compressedData.data.get(), compressedData.size);

		// Decompressing the compressed data
		AssetData uncompressedData;
		uncompressedData.size = entry.packageEntry.size;
		uncompressedData.data = std::make_unique<char[]>(entry.packageEntry.size);

		int uncompressedSize = LZ4_decompress_safe(
			compressedData.data.get(), 
			uncompressedData.data.get(), 
			static_cast<int>(compressedData.size), 
			static_cast<int>(uncompressedData.size)
		);

		if (uncompressedSize < 0) {
			// Decompression failed
			std::cerr << "PackageManager::Unpack(): Decompression failed for " << entry.key << std::endl;
			continue;
		}

		std::ofstream out(filePath, std::ios::binary);
		out.write(uncompressedData.data.get(), uncompressedData.size);

		if (DEBUG) {
			std::cout << "Unpacked " << entry.key << " (" << compressedData.size << " -> " << uncompressedData.size << " bytes)" << std::endl;
		}

		if (!GuidUtils::CreateMetaFileFromGuid(filePath, entry.guid)) {
			std::cerr << "PackageManager::Unpack(): Could not create meta file for " << entry.key << std::endl;
			continue;
		}
	}

	return true;
}

bool PackageManager::MountPackage(const std::string& source)
{
	fs::path sourcePath(source);

	// Path validity checks
	if (!fs::exists(sourcePath)) {
		std::cerr << "PackageManager::MountPackage(): Source does not exist" << std::endl;
		return false;
	}

	if (!fs::is_regular_file(sourcePath)) {
		std::cerr << "PackageManager::MountPackage(): Source is not a file" << std::endl;
		return false;
	}

	// Read file
	std::ifstream in(sourcePath, std::ios::binary);
	if (!in) {
		std::cerr << "PackageManager::MountPackage(): Could not read file" << std::endl;
		return false;
	}

	PackageHeader header;
	in.read(reinterpret_cast<char*>(&header), sizeof(header));

	// Signature check
	if (std::memcmp(header.signature, SIGNATURE, sizeof(header.signature)) != 0) {
		std::cerr << "PackageManager::MountPackage(): Signature does not match" << std::endl;
		return false;
	}

	MountedPackage mountedPackage;

	// Loop through toc and collect all entries
	in.seekg(header.tableOfContentsOffset);
	for (int i = 0; i < header.AssetCount; i++) {
		TOCEntry entry;

		// The GUID
		std::string guid;
		guid.resize(GUID_STR_LENGTH);
		in.read(guid.data(), GUID_STR_LENGTH);

		// The length of the key
		uint32_t keyLength;
		in.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

		// The key
		std::string key;
		key.resize(keyLength);
		in.read(key.data(), keyLength);

		// The entry data (PackageEntry)
		in.read(reinterpret_cast<char*>(&entry.packageEntry), sizeof(entry.packageEntry));

		mountedPackage.tocByPath.emplace(key, entry.packageEntry);
		mountedPackage.tocByGuid.emplace(guid, entry.packageEntry);
	}

	mountedPackage.openFile = std::move(in);
	std::string packageKey = sourcePath.stem().generic_string();
	_mountedPackages.emplace(packageKey, std::move(mountedPackage)); // Have to use std::move as mountedPackage is non-copyable
	_mountOrder.push_back(packageKey);

	if (DEBUG) {
		std::cout << "Mounted package: " << packageKey << std::endl;
	}

	return true;
}

bool PackageManager::UnmountPackage()
{
	// Removing the mounted package
	if (_mountOrder.empty()) {
		std::cerr << "PackageManager::UnmountPakckage(): No packages are currently mounted" << std::endl;
		return false;
	}

	std::string packageKey = _mountOrder.back();

	auto packagePair = _mountedPackages.find(packageKey);
	if (packagePair == _mountedPackages.end()) {
		std::cerr << "PackageManager::LoadAsset(): No package with matching key has been mounted" << std::endl;
		return false;
	}
	_mountedPackages.erase(packagePair);

	// Removing the mounted package key from mounting order
	_mountOrder.pop_back();

	if (DEBUG) {
		std::cout << "Unmounted package: " << packageKey << std::endl;
	}

	return true;
}

bool PackageManager::UnmountPackage(const std::string& packageKey)
{
	// Removing the mounted package
	auto packagePair = _mountedPackages.find(packageKey);
	if (packagePair == _mountedPackages.end()) {
		std::cerr << "PackageManager::LoadAsset(): No package with matching key has been mounted" << std::endl;
		return false;
	}
	_mountedPackages.erase(packagePair);

	// Removing the mounted package key from mounting order
	auto newEnd = std::remove(_mountOrder.begin(), _mountOrder.end(), packageKey);
	_mountOrder.erase(newEnd, _mountOrder.end());

	if (DEBUG) {
		std::cout << "Unmounted package: " << packageKey << std::endl;
	}

	return true;
}

bool PackageManager::LoadAssetByGuid(const std::string& guid, AssetData& asset)
{
	for (size_t i = _mountOrder.size(); i != 0; --i) {
		std::string packageKey = _mountOrder.at(i);

		MountedPackage& mountedPackage = _mountedPackages.find(packageKey)->second;
		auto pair = mountedPackage.tocByGuid.find(guid);
		if (pair != mountedPackage.tocByGuid.end()) {
			// Asset found -> load it
			if (!LoadAsset(mountedPackage, pair->second, asset)) {
				std::cerr << "PackageManager::LoadAssetByGuid(): Unable to load asset" << std::endl;
				return false;
			}

			if (DEBUG) {
				std::cout << "Loaded asset with GUID: " << guid << std::endl;
			}
			return true;
		}
	}

	std::cerr << "PackageManager::LoadAssetByGuid(): Asset does not exist within a mounted package" << std::endl;
	return false;
}

bool PackageManager::LoadAssetByPath(const std::string& path, AssetData& asset)
{
	for (size_t i = _mountOrder.size(); i != 0; --i) {
		std::string packageKey = _mountOrder.at(i);

		MountedPackage& mountedPackage = _mountedPackages.find(packageKey)->second;
		auto pair = mountedPackage.tocByPath.find(path);
		if (pair != mountedPackage.tocByPath.end()) {
			// Asset found -> load it
			if (!LoadAsset(mountedPackage, pair->second, asset)) {
				std::cerr << "PackageManager::LoadAssetByPath(): Unable to load asset" << std::endl;
				return false;
			}

			if (DEBUG) {
				std::cout << "Loaded asset with path: " << path << std::endl;
			}
			return true;
		}
	}

	std::cerr << "PackageManager::LoadAssetByPath(): Asset does not exist within a mounted package" << std::endl;
	return false;
}
