#pragma once

#include "Resource.h"

#include "raylib.h"

class TextureResource : public Resource
{
private:
	Texture2D _texture;

public:

	TextureResource *LoadFromDisk(std::string path) override;
	void UnLoad() override;
	Texture2D GetTexture();
};

