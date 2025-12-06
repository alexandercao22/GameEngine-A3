#include "TextureResource.h"

TextureResource *TextureResource::LoadFromDisk(std::string path)
{
	Resource::RefAdd();

	_texture = LoadTexture(path.c_str());
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
