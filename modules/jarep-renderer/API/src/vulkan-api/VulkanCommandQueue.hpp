//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANCOMMANDQUEUE_HPP
#define JAREP_VULKANCOMMANDQUEUE_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanCommandBuffer;

	class VulkanDevice;

	class VulkanCommandQueue final : public JarCommandQueue {
		public:
			VulkanCommandQueue(std::shared_ptr<VulkanDevice>& device, VkCommandPool commandPool,
			                   std::vector<VulkanCommandBuffer*> commandBuffers) : m_device(device),
			                                                                       m_commandPool(commandPool),
			                                                                       m_commandBuffers(
					                                                                       std::move(commandBuffers)) {
				m_currentBufferIndexInUse = 0;
			};

			~VulkanCommandQueue() override;

			JarCommandBuffer* getNextCommandBuffer() override;

			VkCommandPool GetCommandPool() { return m_commandPool; }

			void Release() override;

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkCommandPool m_commandPool;
			std::vector<VulkanCommandBuffer*> m_commandBuffers;

			uint32_t m_currentBufferIndexInUse;
	};
}

#endif //JAREP_VULKANCOMMANDQUEUE_HPP
