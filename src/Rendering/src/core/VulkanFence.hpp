//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Core {
	/**
	 * GPU-CPU synchronization primitive used to wait for GPU operations to complete from the CPU side.
	 */
	class VulkanFence {
		public:
			VulkanFence(VkDevice device, bool signaled = false);

			~VulkanFence();

			VulkanFence(const VulkanFence&) = delete;

			VulkanFence& operator=(const VulkanFence&) = delete;

			VkFence get() const;

			void wait(uint64_t timeout = UINT64_MAX) const;

			void reset() const;

			bool isSignaled() const;

		private:
			VkDevice m_device;
			VkFence m_fence;
	};
}
