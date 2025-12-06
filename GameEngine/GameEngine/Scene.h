#pragma once

#include <vector>
#include <string>
#include "raylib.h"

class Scene
{
private:
	std::vector<std::string> _GUIDs; // GUIDs needed for this specific scene

	Camera3D _camera = { 0 };
	bool _showCursor = true;

public:
	Scene() = default;
	~Scene() = default;

	bool Init(unsigned int width = 1280, unsigned int height = 720);
	bool Update();
	bool RenderUpdate();
};

