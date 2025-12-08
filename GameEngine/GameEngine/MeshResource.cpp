#include "MeshResource.h"

#include <iostream>

MeshResource* MeshResource::LoadFromDisk(std::string path) {
	//Resource::RefAdd();
	
#ifdef DEBUG
#endif
	std::cout << path << std::endl;

	//Model model = LoadModel(path.c_str());
	_model = LoadModel(path.c_str());
	if (!IsModelValid(_model)) {
		std::cerr << "MeshResource::LoadFromDisk(): Mesh was invalid: " << path << std::endl;
		return nullptr;
	}
	//_model = model;
	return this;
}

Model MeshResource::GetModel() {
	return _model;
}

void MeshResource::UnLoad() {
	UnloadModel(_model);
}