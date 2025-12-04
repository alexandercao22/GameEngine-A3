#include "MeshResource.h"

MeshResource* MeshResource::LoadFromDisk(std::string path) {
	Model model = LoadModel(path.c_str());
	_model = model;
	return this;
}

Model MeshResource::GetModel() {
	return _model;
}

void MeshResource::UnLoad() {
	
}