#include "TextureResource.h"

#include <iostream>

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
