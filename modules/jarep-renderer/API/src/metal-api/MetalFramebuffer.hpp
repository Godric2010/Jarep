//
// Created by Sebastian Borsch on 13.07.24.
//

#ifndef JAREP_METALFRAMEBUFFER_HPP
#define JAREP_METALFRAMEBUFFER_HPP

#include "IRenderAPI.hpp"
#include "MetalImageBuffer.hpp"

namespace Graphics::Metal {
	class MetalImageBuffer;
	class MetalFramebuffer final : public JarFramebuffer {

		public:
			MetalFramebuffer(std::shared_ptr<MetalImageBuffer> targetImageBuffer);

			~MetalFramebuffer() override = default;

			void Release() override;

			std::shared_ptr<MetalImageBuffer> GetRenderTargetBuffer() const { return m_renderTargetBuffer; }

		private:
			std::shared_ptr<MetalImageBuffer> m_renderTargetBuffer;
	};
}

#endif //JAREP_METALFRAMEBUFFER_HPP
