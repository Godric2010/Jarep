//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP
#define JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP

#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanFramebuffer.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>
#include <vector>
#include <optional>

namespace Graphics::Vulkan {
	class VulkanSurface;

	class VulkanFramebuffer;

	class VulkanDevice;

	class VulkanRenderPassFramebuffers {
		public:
			VulkanRenderPassFramebuffers();

			~VulkanRenderPassFramebuffers() = default;

			void
			CreateRenderPassFramebuffers(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSurface> surface,
			                             VkRenderPass renderPass, VkSampleCountFlagBits multisamplingCount,
			                             std::optional<VkFormat> depthFormat);

			std::shared_ptr<VulkanFramebuffer> GetFramebuffer(uint32_t index);

			void RecreateFramebuffers(VkExtent2D swapchainExtent, std::shared_ptr<VulkanSurface> surface);

			void Release();

		private:
			std::shared_ptr<VulkanDevice> m_device;
			std::vector<std::shared_ptr<VulkanFramebuffer>> m_framebuffers;

			VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;

			VkFormat m_depthFormat;
			VkImage m_depthImage;
			VkDeviceMemory m_depthImageMemory;
			VkImageView m_depthImageView;

			VkFormat m_colorFormat;
			VkImage m_colorImage;
			VkDeviceMemory m_colorImageMemory;
			VkImageView m_colorImageView;

			void createDepthResources(VkExtent2D swapchainExtent, VkFormat depthFormat);

			void createColorResources(VkExtent2D swapchainExtent, VkFormat colorFormat);

			void destroyDepthResources();

			void destroyColorResources();
	};
}

#endif //JAREP_VULKANRENDERPASSFRAMEBUFFERS_HPP
