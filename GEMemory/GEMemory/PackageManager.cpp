#include "PackageManager.h"
#include <filesystem>
#include "Settings.h"

namespace fs = std::filesystem;

bool PackageManager::Pack(const std::string& source, const std::string& target)
{
	fs::path sourcePath(source);
	fs::path targetPath(target);

	// Path validity checks
	if (!fs::exists(sourcePath)) {
		std::cerr << "PackageManager::Pack(): Source does not exist" << std::endl;
		return false;
	}

	if (!fs::exists(targetPath)) {
		std::cerr << "PackageManager::Pack(): Target does not exist" << std::endl;
		return false;
	}

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
	if (!fs::exists(sourcePath)) {
		std::cerr << "PackageManager::Unpack(): Source does not exist" << std::endl;
		return false;
	}

	if (!fs::exists(targetPath)) {
		std::cerr << "PackageManager::Unpack(): Target does not exist" << std::endl;
		return false;
	}

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

		// The length of the key
		uint32_t keyLength;
		in.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

		// The key
		std::string key;
		key.resize(keyLength);
		in.read(key.data(), keyLength);

		// The entry data (PackageEntry)
		in.read(reinterpret_cast<char*>(&entry.packageEntry), sizeof(entry.packageEntry));
		
		toc.push_back(entry);
	}

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

		// The length of the key
		uint32_t keyLength;
		in.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

		// The key
		std::string key;
		key.resize(keyLength);
		in.read(key.data(), keyLength);

		// The entry data (PackageEntry)
		in.read(reinterpret_cast<char*>(&entry.packageEntry), sizeof(entry.packageEntry));

		mountedPackage.tableOfContents.emplace(key, entry.packageEntry);
	}

	mountedPackage.openFile = std::move(in);
	std::string packageKey = sourcePath.stem().generic_string();
	_mountedPackages.emplace(packageKey, std::move(mountedPackage)); // Have to use std::move as mountedPackage is non-copyable


	if (DEBUG) {
		std::cout << "Mounted package: " << packageKey << std::endl;
	}

	return true;
}

bool PackageManager::UnmountPackage(const std::string& packageKey)
{
	// Search the mounted packages with packageKey
	auto packagePair = _mountedPackages.find(packageKey);
	if (packagePair == _mountedPackages.end()) {
		std::cerr << "PackageManager::LoadAsset(): No package with matching key has been mounted" << std::endl;
		return false;
	}
	_mountedPackages.erase(packagePair);

	if (DEBUG) {
		std::cout << "Unmounted package: " << packageKey << std::endl;
	}

	return true;
}

bool PackageManager::LoadAsset(const std::string& assetKey, const std::string& packageKey, AssetData& asset)
{
	// Search the mounted packages with packageKey
	auto packagePair = _mountedPackages.find(packageKey);
	if (packagePair == _mountedPackages.end()) {
		std::cerr << "PackageManager::LoadAsset(): No package with matching key has been mounted" << std::endl;
		return false;
	}
	MountedPackage& mountedPackage = packagePair->second;

	// Search the mounted package for the assetKey (path_within_package\filename.extension)
	auto entryPair = mountedPackage.tableOfContents.find(assetKey);
	if (entryPair == mountedPackage.tableOfContents.end()) {
		std::cerr << "PackageManager::LoadAsset(): No asset with matching key exists within the mounted package" << std::endl;
		return false;
	}
	PackageEntry packageEntry = entryPair->second;

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

	if (DEBUG) {
		std::cout << "Loaded asset: " << assetKey << std::endl;
	}

	return true;
}
