//
// Created by Sebastian Borsch on 12.06.24.
//

#ifndef JAREP_METALRENDERTARGET_HPP
#define JAREP_METALRENDERTARGET_HPP

#include "IRenderAPI.hpp"

namespace Graphics::Metal {
	class MetalRenderTarget final : public JarRenderTarget {

		public:
			MetalRenderTarget(RenderTargetType type, uint32_t width, uint32_t height, PixelFormat format) : m_type(
					type), m_width(width), m_height(height), m_pixelFormat(format) {
			}

			~MetalRenderTarget() override;

			inline const uint32_t GetResolutionWidth() override { return m_width; }

			inline const uint32_t GetResolutionHeight() override { return m_height; }

			inline const PixelFormat GetPixelFormat() override { return m_pixelFormat; }

		private:
			RenderTargetType m_type;
			uint32_t m_width;
			uint32_t m_height;
			PixelFormat m_pixelFormat;
	};
}

#endif //JAREP_METALRENDERTARGET_HPP
