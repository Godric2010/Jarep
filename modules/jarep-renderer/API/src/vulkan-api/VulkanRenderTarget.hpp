//
// Created by sebastian on 14.06.24.
//

#ifndef JAREP_VULKANRENDERTARGET_HPP
#define JAREP_VULKANRENDERTARGET_HPP

#include "IRenderAPI.hpp"
#include <vulkan/vulkan_core.h>

namespace Graphics::Vulkan {
	class VulkanRenderTarget final : public JarRenderTarget {

		public:
			VulkanRenderTarget(uint32_t width, uint32_t height, VkFormat imageFormat, RenderTargetType type)
					: m_imageWidth(width), m_imageHeight(height), m_imageFormat(imageFormat),
					  m_renderTargetType(type) {}

			~VulkanRenderTarget() override = default;

			inline const uint32_t GetImageWidth() const { return m_imageWidth; }

			inline const uint32_t GetImageHeight() const { return m_imageHeight; }

			inline const VkFormat GetImageFormat() const { return m_imageFormat; }

			inline const RenderTargetType GetRenderTargetType() const { return m_renderTargetType; }

		private:
			uint32_t m_imageWidth;
			uint32_t m_imageHeight;
			VkFormat m_imageFormat;
			RenderTargetType m_renderTargetType;
	};
}

#endif //JAREP_VULKANRENDERTARGET_HPP
