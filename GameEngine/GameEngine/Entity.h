#pragma once

#include <string>
#include <vector>
#include "raylib.h"

class Entity
{
private:
	Transform _transform = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }
	};

	std::vector<std::string> _GUIDs;

public:
	Entity() = default;
	~Entity() = default;

	void AddGUID(std::string guid);

	Transform *GetTransform();
	std::vector<std::string> GetGUIDs();
};

