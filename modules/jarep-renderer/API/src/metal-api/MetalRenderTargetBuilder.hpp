//
// Created by Sebastian Borsch on 12.06.24.
//

#ifndef JAREP_METALRENDERTARGETBUILDER_HPP
#define JAREP_METALRENDERTARGETBUILDER_HPP

#include "IRenderAPI.hpp"
#include "MetalRenderTarget.hpp"
#include "MetalDevice.hpp"
#include <optional>
#include <memory>
#include <stdexcept>

namespace Graphics::Metal {
	class MetalRenderTargetBuilder final : public Graphics::JarRenderTargetBuilder {

		public:
			MetalRenderTargetBuilder() = default;

			~MetalRenderTargetBuilder() override;

			MetalRenderTargetBuilder* SetRenderTargetType(RenderTargetType type) override;

			MetalRenderTargetBuilder* SetResolution(uint32_t width, uint32_t height) override;

			MetalRenderTargetBuilder* SetImageFormat(PixelFormat format) override;

			std::shared_ptr<JarRenderTarget> Build() override;

		private:
			std::optional<RenderTargetType> m_renderTargetType;
			std::optional<uint32_t> m_width;
			std::optional<uint32_t> m_height;
			std::optional<PixelFormat> m_pixelFormat;
	};
}


#endif //JAREP_METALRENDERTARGETBUILDER_HPP
