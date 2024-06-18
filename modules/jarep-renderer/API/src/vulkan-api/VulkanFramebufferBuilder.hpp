//
// Created by sebastian on 18.06.24.
//

#ifndef JAREP_VULKANFRAMEBUFFERBUILDER_HPP
#define JAREP_VULKANFRAMEBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDataTypeMaps.hpp"
#include "VulkanRenderPass.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanFramebufferBuilder : public JarFramebufferBuilder {

		public:
			VulkanFramebufferBuilder() = default;

			~VulkanFramebufferBuilder() override = default;

			VulkanFramebufferBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			VulkanFramebufferBuilder* SetFramebufferExtent(uint32_t width, uint32_t height) override;

			VulkanFramebufferBuilder* SetImageFormat(PixelFormat format) override;

			std::shared_ptr<JarFramebuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::shared_ptr<VulkanRenderPass> m_renderPass;
			VkExtent2D m_framebufferExtent;
			VkFormat m_imageFormat;
	};
}

#endif //JAREP_VULKANFRAMEBUFFERBUILDER_HPP
