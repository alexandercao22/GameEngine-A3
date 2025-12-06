#include "TextureResource.h"

TextureResource *TextureResource::LoadFromDisk(std::string path)
{
	Resource::RefAdd();

	_texture = LoadTexture(path.c_str());
	return this;
}

void TextureResource::UnLoad()
{
}

Texture2D TextureResource::GetTexture()
{
	return Texture2D();
}
