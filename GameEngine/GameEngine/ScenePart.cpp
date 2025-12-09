#include "ScenePart.h"

bool ScenePart::Init(float pos[3], std::string path) {
	_centerPos[0] = pos[0];
	_centerPos[1] = pos[1];
	_centerPos[2] = pos[2];
	_pathToPackage = path;
}

std::string ScenePart::GetPath() {
	return _pathToPackage;
}