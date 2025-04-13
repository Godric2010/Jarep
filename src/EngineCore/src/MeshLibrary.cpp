//
// Created by Godri on 4/13/2025.
//

#include "MeshLibrary.hpp"

using namespace JAREP::Core;

MeshLibrary::MeshLibrary() {
	m_meshes = std::map<size_t, Types::Mesh>();
	m_lastIndex = 0;
}

MeshLibrary::~MeshLibrary() {
	m_meshes.clear();
}

size_t MeshLibrary::AddMesh(Types::Mesh mesh) {
	size_t newIndex = m_lastIndex + 1;
	m_meshes[newIndex] = mesh;
	return newIndex;
}

Types::Mesh MeshLibrary::GetMesh(size_t index) {
	return m_meshes[index];
}
