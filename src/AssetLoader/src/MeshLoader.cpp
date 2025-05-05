//
// Created by Godri on 4/13/2025.
//

#include "MeshLoader.hpp"

using namespace JAREP::AssetLoader;

MeshLoader::MeshLoader() = default;

MeshLoader::~MeshLoader() = default;

JAREP::Core::Types::Mesh MeshLoader::LoadMeshAsObj(std::string&meshName) {
	std::string fullPath = "Resources/Meshes/" + meshName + ".obj";
	Core::Types::Mesh mesh;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullPath.c_str(), nullptr, true);
	if (!warn.empty()) {
		std::cerr << "TinyObjLoader Warning: " << warn << "\n" << std::endl;
	}
	if (!err.empty()) {
		std::cerr << "TinyObjLoader Error: " << err << "\n" << std::endl;
	}
	if (!ret) {
		throw std::runtime_error("Failed to load .obj file: " + meshName);
	}

	for (const auto&shape: shapes) {
		for (const auto&index: shape.mesh.indices) {
			Core::Types::Vertex vertex{};

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			// if (!attrib.texcoords.empty()) {
			if (index.texcoord_index >= 0){
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}
			else {
				vertex.uv = {0.0f, 0.0f};
			}


			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(static_cast<uint32_t>(mesh.vertices.size() - 1));
		}
	}


	return mesh;
}
