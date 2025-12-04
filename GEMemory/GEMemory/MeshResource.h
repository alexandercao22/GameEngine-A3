#pragma once
#include "Resource.h"
#include "Raylib.h"
#include <string>
class MeshResource : public Resource {
private: 
	//Mesh mesh;
	Model _model;

public:
	//~MeshResource() = default;
	
	MeshResource* LoadFromDisk(std::string GUID) override;
	void UnLoad() override;
	
	Model GetModel();
};