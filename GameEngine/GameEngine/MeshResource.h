#pragma once
#include "Resource.h"
#include "Raylib.h"
#include <string>
class MeshResource : public Resource {
private: 
	Model _model;

public:
	MeshResource* LoadFromDisk(std::string GUID) override;
	void UnLoad() override;
	Model GetModel();
};