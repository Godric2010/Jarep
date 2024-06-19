//
// Created by Sebastian Borsch on 19.06.24.
//

#ifndef JAREP_METALFRAMEBUFFERBUILDER_HPP
#define JAREP_METALFRAMEBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"

namespace Graphics::Metal {

	class MetalFramebufferBuilder final : public JarFramebufferBuilder {
		public:
			MetalFramebufferBuilder() = default;

			~MetalFramebufferBuilder() override = default;

			MetalFramebufferBuilder* SetFramebufferExtent(uint32_t width, uint32_t height) override;

			MetalFramebufferBuilder* SetImageFormat(PixelFormat format) override;

			MetalFramebufferBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			std::shared_ptr<JarFramebuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<uint32_t> m_width;
			std::optional<uint32_t> m_height;
			std::optional<PixelFormat> m_pixelFormat;
			std::optional<std::shared_ptr<JarRenderPass>> m_renderPass;
	};
}

#endif //JAREP_METALFRAMEBUFFERBUILDER_HPP
