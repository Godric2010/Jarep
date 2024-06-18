//
// Created by sebastian on 14.06.24.
//

#ifndef JAREP_VULKANRENDERTARGETBUILDER_HPP
#define JAREP_VULKANRENDERTARGETBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanDataTypeMaps.hpp"
#include <vulkan/vulkan_core.h>
#include <memory>
#include <optional>

namespace Graphics::Vulkan {
	class VulkanRenderTargetBuilder final : public JarRenderTargetBuilder {

		public:
			VulkanRenderTargetBuilder() = default;

			~VulkanRenderTargetBuilder() override = default;

			VulkanRenderTargetBuilder* SetRenderTargetType(Graphics::RenderTargetType renderTargetType) override;

			VulkanRenderTargetBuilder* SetImageFormat(Graphics::PixelFormat pixelFormat) override;

			VulkanRenderTargetBuilder* SetResolution(uint32_t width, uint32_t height) override;

			std::shared_ptr<JarRenderTarget> Build() override;

		private:
			std::optional<uint32_t> m_width;
			std::optional<uint32_t> m_height;
			std::optional<PixelFormat> m_format;
			std::optional<RenderTargetType> m_type;
	};
}

#endif //JAREP_VULKANRENDERTARGETBUILDER_HPP
