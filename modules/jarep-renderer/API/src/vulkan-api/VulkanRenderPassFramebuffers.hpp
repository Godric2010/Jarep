//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP
#define JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP

#include "VulkanCommandQueue.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFramebuffer_OLD.hpp"
#include "VulkanImageBuffer.hpp"
#include "VulkanSurface.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanSurface;

	class VulkanFramebuffer_OLD;

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

			std::shared_ptr<VulkanFramebuffer_OLD> GetFramebuffer(uint32_t index);

			void RecreateFramebuffers(VkExtent2D swapchainExtent, const std::shared_ptr<VulkanSurface>& surface);

			void Release();

		private:
			std::shared_ptr<VulkanDevice> m_device;
			std::vector<std::shared_ptr<VulkanFramebuffer_OLD>> m_framebuffers;

			VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;

			std::function<std::shared_ptr<VulkanCommandQueue>()> m_createCmdQueueCallback;
	};
}

#endif //JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP
