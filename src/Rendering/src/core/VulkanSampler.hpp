//
// Created by Godri on 3/26/2025.
//

#pragma once
#include <Rendering/Core/VulkanSamplerConfig.hpp>
#include <vulkan/vulkan.hpp>


namespace JAREP::Rendering::Core {
	/**
	 * Encapsulates Vulkan sampler creation and configuration. Supports predefined presets and custom settings.
	 */
	class VulkanSampler {
		public:
			VulkanSampler(VkDevice device, VkPhysicalDevice physicalDevice, const SamplerConfig&config);

			~VulkanSampler();

			VkSampler get() const;

		private:
			VkDevice m_device;
			VkSampler m_sampler;
	};
} // Core
