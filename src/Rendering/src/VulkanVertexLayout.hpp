//
// Created by Godri on 4/20/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include "EngineCore/Types/Vertex.hpp"

namespace JAREP::Rendering {
	struct VulkanVertexLayout {
		static VkVertexInputBindingDescription GetBindingDescription();

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	};
}
