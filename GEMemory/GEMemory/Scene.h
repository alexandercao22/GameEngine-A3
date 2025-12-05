#pragma once

#include "ResourceManager.h"

#include <vector>
#include "raylib.h"

class Scene
{
private:
	ResourceManager _resourceManager;
	std::vector<Resource *> _resources;

	Camera3D _camera = { 0 };
	bool _showCursor = true;

	// Temporary
	Mesh _cubeMesh;
	Model _duck;
	Model _cubeModel;

public:
	Scene() = default;
	~Scene() = default;

	bool Init(unsigned int width = 1280, unsigned int height = 720);
	void Update();
};

