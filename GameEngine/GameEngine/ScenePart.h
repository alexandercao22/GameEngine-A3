#pragma once
#include <string>
#include "raylib.h"

// This is a class that Scene will hold to demonstrate asynchronous loading

class ScenePart {
private:
	std::string _pathToPackage;
	Vector3 _centerPos = { 0,0,0 };
	bool _loaded = false;

public:
	bool Init(Vector3 pos, std::string path);
	bool CheckDistance(Vector3 camera);
	bool IsLoaded();
	std::string GetPath();
};