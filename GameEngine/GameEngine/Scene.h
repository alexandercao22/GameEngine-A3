#pragma once

#include "Entity.h"
#include "EntityEnemy.h"

#include <vector>
#include <string>
#include "raylib.h"

class Scene
{
private:
	std::vector<std::string> _GUIDs; // GUIDs needed for this specific scene

	std::vector<Entity *> _entities;

	Camera3D _camera = { 0 };
	bool _showCursor = true;
	unsigned int _width = 1280;
	unsigned int _height = 720;

	bool RenderInterface();

public:
	Scene() = default;
	~Scene();

	bool Init(unsigned int width = 1280, unsigned int height = 720);
	bool Update();
	bool RenderUpdate();
};

