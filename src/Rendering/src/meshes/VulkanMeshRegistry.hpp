//
// Created by Godri on 4/14/2025.
//

#pragma once
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "VulkanMesh.hpp"
#include "EngineCore/IMeshLibrary.hpp"
#include "EngineCore/Types/Mesh.hpp"


namespace JAREP::Rendering::Meshes {
	class VulkanMeshRegistry {
		public:
			VulkanMeshRegistry(VkDevice device, VkPhysicalDevice physicalDevice);

			~VulkanMeshRegistry();

			VulkanMeshRegistry(const VulkanMeshRegistry&) = delete;

			VulkanMeshRegistry& operator=(const VulkanMeshRegistry&) = delete;

			void AddMesh(const JAREP::Core::Types::Mesh&mesh, JAREP::Core::MeshID meshID);

			bool HasMesh(JAREP::Core::MeshID meshID) const;

			[[nodiscard]] std::optional<VulkanMesh*> TryGetMesh(JAREP::Core::MeshID meshID);

			void RemoveMesh(JAREP::Core::MeshID meshID);

		private:
			std::unordered_map<JAREP::Core::MeshID, std::unique_ptr<VulkanMesh>> m_meshes;

			VkDevice m_device;
			VkPhysicalDevice m_physicalDevice;
	};
}
