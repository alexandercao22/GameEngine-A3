#include "MeshResource.h"

#include <iostream>
#include "raylib.h"
#include "OBJ_Loader.h"

bool MeshResource::Init()
{
	objl::Loader objLoader;

	// Parse obj data
	if (!objLoader.LoadData(std::string(_data.data.get()))) {
		std::cerr << "MeshResource::Init(): Failed to load mesh from data" << std::endl;
		return false;
	}

	objl::Mesh objMesh = objLoader.LoadedMeshes[0];

	// Convert to raylib mesh data
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	for (objl::Vertex &vertex : objMesh.Vertices) {
		positions.push_back(vertex.Position.X);
		positions.push_back(vertex.Position.Y);
		positions.push_back(vertex.Position.Z);

		normals.push_back(vertex.Normal.X);
		normals.push_back(vertex.Normal.Y);
		normals.push_back(vertex.Normal.Z);

		uvs.push_back(vertex.TextureCoordinate.X);
		uvs.push_back(vertex.TextureCoordinate.Y);
	}

	Mesh mesh{};
	mesh.vertexCount = objMesh.Vertices.size();
	mesh.indices = objMesh.Indices.data();
	mesh.vertices = positions.data();
	mesh.normals = normals.data();
	mesh.texcoords = uvs.data();
	mesh.triangleCount = objMesh.Vertices.size() / 3;
	UploadMesh(&mesh, true);

	_model = LoadModelFromMesh(mesh);
	if (!IsModelValid(_model)) {
		std::cerr << "MeshResource::Init(): Loaded model was invalid" << std::endl;
		return false;
	}

	return true;
}

MeshResource* MeshResource::LoadFromDisk(std::string path) {
	Resource::RefAdd();
	
	Model model = LoadModel(path.c_str());
	if (!IsModelValid(model)) {
		std::cerr << "MeshResource::LoadFromDisk(): Mesh was invalid: " << path << std::endl;
		return nullptr;
	}
	_model = model;
	return this;
}

Model MeshResource::GetModel() {
	return _model;
}

void MeshResource::UnLoad() {
	UnloadModel(_model);
}