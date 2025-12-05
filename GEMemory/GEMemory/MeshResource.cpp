#include "MeshResource.h"

MeshResource* MeshResource::LoadFromDisk(std::string path) {
	Resource::RefAdd();
	
	Model model = LoadModel(path.c_str());
	_model = model;
	return this;
}

Model MeshResource::GetModel() {
	return _model;
}

int MeshResource::GetRef() {
	return Resource::GetRef();
}

void MeshResource::UnLoad() {
	UnloadModel(_model);

}