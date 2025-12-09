#pragma once
#include <string>

// This is a class that Scene will hold to demonstrate asynchronous loading

class ScenePart {
private:
	std::string _pathToPackage;
	float _centerPos[3] = { 0,0,0 };

public:
	bool Init(float pos[3], std::string path);
	std::string GetPath();
};