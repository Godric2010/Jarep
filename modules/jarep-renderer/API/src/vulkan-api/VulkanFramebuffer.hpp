//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANFRAMEBUFFER_HPP
#define JAREP_VULKANFRAMEBUFFER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanFramebuffer final {
		public:
			explicit VulkanFramebuffer(const VkExtent2D framebufferExtent) {
				m_framebuffer = nullptr;
				m_framebufferExtent = framebufferExtent;
			}

			~VulkanFramebuffer();

			void CreateFramebuffer(std::shared_ptr<VulkanDevice> device, VkRenderPass renderPass,
			                       std::vector<VkImageView> vulkanImageAttachments);

			void RecreateFramebuffer(uint32_t width, uint32_t height, VkImageView swapchainImageView,
			                         VkImageView depthImageView, VkImageView colorImageView);

			[[nodiscard]] VkFramebuffer getFramebuffer() const { return m_framebuffer; }

			[[nodiscard]] VkExtent2D getFramebufferExtent() const { return m_framebufferExtent; }

			void Release();

		private:
			VkFramebufferCreateInfo m_framebufferCreateInfo{};
			VkFramebuffer m_framebuffer;
			VkExtent2D m_framebufferExtent{};
			VkRenderPass m_renderPass;
			std::shared_ptr<VulkanDevice> m_device;

			void buildFramebuffer(std::vector<VkImageView> attachments);
	};
}


#endif //JAREP_VULKANFRAMEBUFFER_HPP
