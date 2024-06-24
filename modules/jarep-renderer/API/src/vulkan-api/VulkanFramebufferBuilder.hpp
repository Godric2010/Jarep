//
// Created by sebastian on 18.06.24.
//

#ifndef JAREP_VULKANFRAMEBUFFERBUILDER_HPP
#define JAREP_VULKANFRAMEBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDataTypeMaps.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanRenderPass.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanFramebuffer;

	class VulkanDevice;

	class VulkanFramebufferBuilder : public JarFramebufferBuilder {

		public:
		VulkanFramebufferBuilder() = default;

		~VulkanFramebufferBuilder() override = default;

		VulkanFramebufferBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

		VulkanFramebufferBuilder* SetFramebufferExtent(uint32_t width, uint32_t height) override;

		VulkanFramebufferBuilder* SetImageFormat(PixelFormat format) override;

		VulkanFramebufferBuilder* SetImageBuffers(std::vector<std::shared_ptr<JarImageBuffer>> imageBuffers) override;

		std::shared_ptr<JarFramebuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
		std::optional<std::shared_ptr<VulkanRenderPass>> m_renderPass;
		std::optional<VkExtent2D> m_framebufferExtent;
		std::optional<VkFormat> m_imageFormat;
		std::vector<VkImageView> m_imageViews;
		std::optional<std::shared_ptr<VulkanImageBuffer>> m_targetImageBuffer;
	};
}// namespace Graphics::Vulkan

#endif//JAREP_VULKANFRAMEBUFFERBUILDER_HPP
