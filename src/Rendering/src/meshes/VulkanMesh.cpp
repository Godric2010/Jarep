//
// Created by Godri on 4/14/2025.
//

#include "VulkanMesh.hpp"

using namespace JAREP::Rendering::Meshes;

VulkanMesh::VulkanMesh(VkDevice device, VkPhysicalDevice physicalDevice,
                       const std::shared_ptr<JAREP::Core::Types::Mesh>& mesh) {
	VkDeviceSize vertexSize = sizeof(JAREP::Core::Types::Vertex) * mesh->vertices.size();
	m_vertexBuffer = std::make_unique<Core::VulkanBuffer>(device, physicalDevice, vertexSize,
	                                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	                                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_vertexBuffer->copyFrom(mesh->vertices.data(), vertexSize);

	m_indexCount = mesh->indices.size();
	VkDeviceSize indexSize = m_indexCount * sizeof(uint32_t);
	m_indexBuffer = std::make_unique<Core::VulkanBuffer>(device, physicalDevice, indexSize,
	                                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_indexBuffer->copyFrom(mesh->indices.data(), indexSize);
}

VulkanMesh::~VulkanMesh() {
	m_vertexBuffer.reset();
	m_indexBuffer.reset();
}

void VulkanMesh::Bind(VkCommandBuffer cmdBuffer) const {
	VkBuffer vertexBuffer = m_vertexBuffer->getBuffer();
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, &offset);

	vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void VulkanMesh::Draw(VkCommandBuffer cmdBuffer) const {
	vkCmdDrawIndexed(cmdBuffer, m_indexCount, 1, 0, 0, 0);
}
