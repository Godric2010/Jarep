//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Core {
	class VulkanSemaphore {
		public:
			explicit VulkanSemaphore(VkDevice device);

			~VulkanSemaphore();

			VkSemaphore get() const;

		private:
			VkDevice m_device;
			VkSemaphore m_semaphore;
	};
}
