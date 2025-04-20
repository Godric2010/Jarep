//
// Created by Godri on 4/14/2025.
//

#pragma once
#include <memory>

#include "../core/VulkanBuffer.hpp"
#include "EngineCore/Types/Mesh.hpp"


namespace JAREP::Rendering::Meshes {
	class VulkanMesh {
		public:
			VulkanMesh(VkDevice device, VkPhysicalDevice physicalDevice, const std::shared_ptr<JAREP::Core::Types::Mesh>& mesh);
			~VulkanMesh();

			void Bind(VkCommandBuffer cmdBuffer) const;
			void Draw(VkCommandBuffer cmdBuffer) const;

		private:

			size_t m_indexCount;
			std::unique_ptr<Core::VulkanBuffer> m_vertexBuffer;
			std::unique_ptr<Core::VulkanBuffer> m_indexBuffer;
	};
}
