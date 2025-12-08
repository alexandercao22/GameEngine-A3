#pragma once
#include "Resource.h"
#include "raylib.h"
#include <string>

class MeshResource : public Resource {
private: 
	Model _model;

public:
	bool Init();
	MeshResource* LoadFromDisk(std::string GUID) override;
	void UnLoad() override;
	Model GetModel();
};