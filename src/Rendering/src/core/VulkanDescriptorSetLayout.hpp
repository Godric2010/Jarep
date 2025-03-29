//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Core {
	/**
	 * Defines the layout of a descriptor set. Specifies the types and binding points of resources used in shaders.
	 */
	class VulkanDescriptorSetLayout {
		public:
			struct BindingInfo {
				uint32_t binding = 0;
				VkDescriptorType type;
				VkShaderStageFlags stageFlags;
				uint32_t count;
			};

			VulkanDescriptorSetLayout(VkDevice device, const std::vector<BindingInfo>&bindings);

			~VulkanDescriptorSetLayout();

			VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;

			VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

			VkDescriptorSetLayout get() const;

		private:
			VkDevice m_device;
			VkDescriptorSetLayout m_layout;
	};
}
