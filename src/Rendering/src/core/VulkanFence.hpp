//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Core {
	class VulkanFence {
		public:
			VulkanFence(VkDevice device, bool signaled = false);

			~VulkanFence();

			VkFence get() const;

			void wait(uint64_t timeout = UINT64_MAX) const;

			void reset() const;

			bool isSignaled() const;

		private:
			VkDevice m_device;
			VkFence m_fence;
	};
}
