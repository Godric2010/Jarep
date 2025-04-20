//
// Created by Godri on 4/14/2025.
//

#include "VulkanMeshRegistry.hpp"

#include <stdexcept>

using namespace JAREP::Rendering::Meshes;

VulkanMeshRegistry::VulkanMeshRegistry(VkDevice device, VkPhysicalDevice physicalDevice) {
	m_device = device;
	m_physicalDevice = physicalDevice;

	m_meshes = std::unordered_map<JAREP::Core::MeshID, std::unique_ptr<VulkanMesh>>();
}

VulkanMeshRegistry::~VulkanMeshRegistry() = default;

void VulkanMeshRegistry::AddMesh(const std::shared_ptr<JAREP::Core::Types::Mesh>&mesh, JAREP::Core::MeshID meshID) {
	if (m_meshes.contains(meshID)) {
		throw std::runtime_error("Mesh already exists");
	}

	m_meshes[meshID] = std::make_unique<VulkanMesh>(m_device, m_physicalDevice, mesh);
}

bool VulkanMeshRegistry::HasMesh(const JAREP::Core::MeshID meshID) const {
	return m_meshes.contains(meshID);
}

std::optional<VulkanMesh *> VulkanMeshRegistry::TryGetMesh(const JAREP::Core::MeshID meshID) {
	const auto it = m_meshes.find(meshID);
	return it != m_meshes.end() ? std::make_optional(it->second.get()) : std::nullopt;
}

void VulkanMeshRegistry::RemoveMesh(const JAREP::Core::MeshID meshID) {
	if (!m_meshes.contains(meshID)) {
		throw std::runtime_error("Mesh does not exist in vulkan mesh registry!");
	}
	m_meshes.erase(meshID);
}
