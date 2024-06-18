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
			VulkanRenderTarget(uint32_t width, uint32_t height, PixelFormat imageFormat, RenderTargetType type)
					: m_imageWidth(width), m_imageHeight(height), m_imageFormat(imageFormat),
					  m_renderTargetType(type) {}

			~VulkanRenderTarget() override = default;

			inline const uint32_t GetResolutionWidth() override { return m_imageWidth; }

			inline const uint32_t GetResolutionHeight() override { return m_imageHeight; }

			inline const PixelFormat GetPixelFormat() override { return m_imageFormat; }

			[[nodiscard]] inline RenderTargetType GetRenderTargetType() const { return m_renderTargetType; }

		private:
			uint32_t m_imageWidth;
			uint32_t m_imageHeight;
			PixelFormat m_imageFormat;
			RenderTargetType m_renderTargetType;
	};
}

#endif //JAREP_VULKANRENDERTARGET_HPP
