#pragma once
#include <iostream>
#include <filesystem>

namespace GuidUtils {
	// Returns a new randomized GUID
	std::string Generate();

	// Gets the GUID of an asset if it has a corresponding .meta file
	// If no .meta file is found, it will be created and a new GUID will be written into it
	// Returns the GUID of the asset
	std::string GetOrGenerate(const std::filesystem::path& assetPath);
}
