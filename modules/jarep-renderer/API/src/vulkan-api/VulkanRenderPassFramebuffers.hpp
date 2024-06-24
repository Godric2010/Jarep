//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP
#define JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP

#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanCommandQueue.hpp"
#include "VulkanImageBuffer.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>
#include <functional>
#include <vector>
#include <optional>

namespace Graphics::Vulkan {
	class VulkanSurface;

	class VulkanFramebuffer;

	class VulkanDevice;

	class VulkanImageBuffer;

	class VulkanCommandQueue;

	class VulkanRenderPassFramebuffers {
		public:
			VulkanRenderPassFramebuffers(std::shared_ptr<VulkanDevice> device,
			                             std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCb);

			~VulkanRenderPassFramebuffers() = default;

			void
			CreateRenderPassFramebuffers(const std::shared_ptr<VulkanSurface>& surface,
			                             VkRenderPass renderPass,
			                             std::vector<std::shared_ptr<JarImageBuffer>> imageAttachments);

			std::shared_ptr<VulkanFramebuffer> GetFramebuffer(uint32_t index);

			void RecreateFramebuffers(VkExtent2D swapchainExtent, const std::shared_ptr<VulkanSurface>& surface);

			void Release();

		private:
			std::shared_ptr<VulkanDevice> m_device;
			std::vector<std::shared_ptr<VulkanFramebuffer>> m_framebuffers;

			VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;

			std::function<std::shared_ptr<VulkanCommandQueue>()> m_createCmdQueueCallback;
	};
}

#endif //JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP
