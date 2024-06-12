//
// Created by Sebastian Borsch on 12.06.24.
//

#ifndef JAREP_METALRENDERTARGET_HPP
#define JAREP_METALRENDERTARGET_HPP

#include "IRenderAPI.hpp"

namespace Graphics::Metal {
	class MetalRenderTarget final : public JarRenderTarget {

		public:
			MetalRenderTarget(RenderTargetType type, uint32_t width, uint32_t height) : m_type(type), m_width(width), m_height(height) {
			}

			~MetalRenderTarget() override;

			void Release() override;

			std::shared_ptr<JarImage> GetImage() override;

		private:
			RenderTargetType m_type;
			uint32_t m_width;
			uint32_t m_height;
	};
}

#endif //JAREP_METALRENDERTARGET_HPP
