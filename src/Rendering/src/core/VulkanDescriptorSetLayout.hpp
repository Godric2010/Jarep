//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Core {
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

			VkDescriptorSetLayout get() const;

		private:
			VkDevice m_device;
			VkDescriptorSetLayout m_layout;
	};
}
