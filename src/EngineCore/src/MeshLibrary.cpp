//
// Created by Godri on 4/13/2025.
//

#include "MeshLibrary.hpp"

#include <stdexcept>

using namespace JAREP::Core;

MeshLibrary::MeshLibrary() {
	m_meshes = std::unordered_map<MeshID, Types::Mesh>();
}

MeshLibrary::~MeshLibrary() {
	m_meshes.clear();
}

MeshID MeshLibrary::LoadMesh(const Types::Mesh mesh) {
	const MeshID id = Hashing::HashMesh(mesh);
	if (m_meshes.contains(id)) {
		return id;
	}
	m_meshes[id] = mesh;
	return id;
}

void MeshLibrary::UnloadMesh(const MeshID id) {
	if (m_meshes.contains(id)) {
		m_meshes.erase(id);
		return;
	}
	throw std::out_of_range("Could not remove mesh from library, since it seems to be not loaded.");
}

Types::Mesh MeshLibrary::GetMesh(const MeshID id) const {
	auto it = m_meshes.find(id);
	if (it != m_meshes.end()) {
		return it->second;
	}
	throw std::out_of_range("Mesh does not exist in library");
}
