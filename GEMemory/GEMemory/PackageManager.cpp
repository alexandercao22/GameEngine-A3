#include "PackageManager.h"
#include <filesystem>
#include "Settings.h"

namespace fs = std::filesystem;

bool PackageManager::Pack(const std::string& path)
{
	fs::path rootPath(path);

	// Path validity checks
	if (!fs::exists(rootPath)) {
		std::cerr << "PackageManager::Pack: Path does not exist" << std::endl;
		return false;
	}

	if (!fs::is_directory(rootPath)) {
		std::cerr << "PackageManager::Pack: Path is not a directory" << std::endl;
		return false;
	}

	// Output file (package)
	std::ofstream out("data.gepak", std::ios::binary);
	if (!out) {
		std::cerr << "PackageManager::Pack(): Unable to create output file" << std::endl;
		return false;
	}

	// Offset writing position to account for header (To be written at the front of the package)
	PackageHeader header = {};
	out.seekp(sizeof(PackageHeader));

	// Table of contents (To be written at the back of the package)
	std::vector<TOCEntry> toc;

	// Packaging directory
	for (const auto& dirEntry : fs::recursive_directory_iterator(rootPath)) {
		if (fs::is_regular_file(dirEntry)) {
			fs::path entryPath = dirEntry.path();
			fs::path relativePath = fs::relative(entryPath, rootPath); // Relative path for TOC entry
			std::string key = relativePath.generic_string();

			// Read file
			std::ifstream in(entryPath, std::ios::binary);
			if (!in) {
				std::cerr << "PackageManager::Pack(): Could not read file" << std::endl;
				return false;
			}

			AssetData fileData;
			in.seekg(0, std::ios::end); // Set cursor to end of file
			fileData.size = static_cast<uint64_t>(in.tellg()); // Get cursor pos
			in.seekg(0, std::ios::beg); // Reset curosr to start of file
			fileData.data = std::make_unique<char[]>(fileData.size);
			in.read(fileData.data.get(), fileData.size); // Filling AssetData with file contents

			AssetData compressedData;
			int maxSizeCompressed = LZ4_compressBound(fileData.size); // Maximum size the compressed version can reach
			compressedData.data = std::make_unique<char[]>(maxSizeCompressed);

			// Compressing file
			int sizeCompressed = LZ4_compress_default(fileData.data.get(), compressedData.data.get(), fileData.size, maxSizeCompressed);
			if (sizeCompressed == 0) {
				std::cerr << "PackageManager::Pack(): Compression error" << std::endl;
				return false;
			}
			compressedData.size = sizeCompressed; // Correct the size of the data buffer

			TOCEntry entry;
			entry.key = key;
			entry.entryData.offset = static_cast<uint64_t>(out.tellp());
			entry.entryData.size = fileData.size;
			entry.entryData.sizeCompressed = static_cast<uint64_t>(sizeCompressed);

			toc.push_back(entry);
			
			// Write the compressed data to the output file
			out.write(compressedData.data.get(), compressedData.size);
		}
	}

	// Finalizing header
	header.tableOfContentsOffset = out.tellp(); // Recording starting offset for the TOC
	header.AssetCount = toc.size();
	std::memcpy(header.signature, SIGNATURE, sizeof(SIGNATURE));

	// Writing table of contents to the back of the package
	for (TOCEntry& entry : toc) {
		// The length of the key
		uint32_t keyLength = static_cast<uint32_t>(entry.key.size());
		out.write(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

		// The key
		out.write(entry.key.data(), keyLength);

		// The entry data (PackageEntry)
		out.write(reinterpret_cast<char*>(&entry.entryData), sizeof(entry.entryData));
	}

	out.seekp(0);
	out.write(reinterpret_cast<char*>(&header), sizeof(header));

	return true;
}

bool PackageManager::Unpack(const std::string& path)
{
	// Check if package has the correct signature
	//
	// Restore the folder to its original state

	return false;
}

bool PackageManager::MountPackage(const std::string& path)
{
	// Check if package has the correck signature
	// 
	// Read header and toc and create a MountedPakcage filled with PackageEntry's and add it to _mountedPackages


	return false;
}

AssetData PackageManager::LoadAsset(const std::string& assetName)
{
	// Search the all mounted packages for the assetName (filename.extension)
	// If it is found, load it into memory and return it as AssetData

	return AssetData();
}
