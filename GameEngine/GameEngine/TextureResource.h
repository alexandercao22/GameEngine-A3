#pragma once

#include "Resource.h"

#include "raylib.h"

class TextureResource : public Resource
{
private:
	Texture2D _texture;

public:
	bool LoadFromData(const char* data, size_t size);
	uint64_t GetMemoryUsage();

	TextureResource *LoadFromDisk(std::string path) override;
	void Unload() override;
	Texture2D GetTexture();
};

