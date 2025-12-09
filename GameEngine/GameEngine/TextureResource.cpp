#include "TextureResource.h"

#include <iostream>

bool TextureResource::LoadFromData(const char* data, size_t size)
{
	Image image = LoadImageFromMemory("png", (const unsigned char*)data, size);
	_texture = LoadTextureFromImage(image);

	if (!IsTextureValid(_texture)) {
		std::cerr << "TextureResource::LoadFromData(): Failed to load texture" << std::endl;
		return false;
	}

	// Calculate memory usage
	size_t textureSize = image.width * image.height * 4;
	_memoryUsage = (uint64_t)textureSize;

	UnloadImage(image);

	return true;
}

uint64_t TextureResource::GetMemoryUsage()
{
	return _memoryUsage;
}

TextureResource *TextureResource::LoadFromDisk(std::string path)
{
	Resource::RefAdd();

	Texture2D texture = LoadTexture(path.c_str());
	if (!IsTextureValid(texture)) {
		std::cerr << "TextureResource::LoadFromDisk(): Texture was invalid: " << path << std::endl;
		return nullptr;
	}
	_texture = texture;
	return this;
}

void TextureResource::UnLoad()
{
	UnloadTexture(_texture);
}

Texture2D TextureResource::GetTexture()
{
	return _texture;
}
