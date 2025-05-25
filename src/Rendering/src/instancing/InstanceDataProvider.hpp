//
// Created by Godri on 5/18/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "Rendering/Core/RenderObject.hpp"
#include "../meshes/VulkanMeshRegistry.hpp"

namespace JAREP::Rendering::Instancing {
	struct InstanceDataProvider {
		virtual ~InstanceDataProvider() = default;

		virtual VkDescriptorSetLayout GetDescriptorSetLayout() const = 0;

		virtual void PrepareData(VkCommandBuffer&cmd,
		                         const std::vector<Core::RenderObject>&objects) = 0;

		virtual void BindData(VkCommandBuffer&cmd, VkPipelineLayout pipelineLayout) = 0;
	};
}
