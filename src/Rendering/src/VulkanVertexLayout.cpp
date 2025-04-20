//
// Created by Godri on 4/20/2025.
//

#include "VulkanVertexLayout.hpp"
#include <cstddef>

using namespace JAREP::Rendering;

VkVertexInputBindingDescription VulkanVertexLayout::GetBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Core::Types::Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> VulkanVertexLayout::GetAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

	// Position
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Core::Types::Vertex, position);

	// UV
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Core::Types::Vertex, uv);

	return attributeDescriptions;
}
