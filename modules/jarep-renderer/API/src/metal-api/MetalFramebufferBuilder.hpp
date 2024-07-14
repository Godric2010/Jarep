//
// Created by Sebastian Borsch on 19.06.24.
//

#ifndef JAREP_METALFRAMEBUFFERBUILDER_HPP
#define JAREP_METALFRAMEBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include "MetalImageBuffer.hpp"
#include "MetalRenderPass.hpp"
#include "MetalImageBufferBuilder.hpp"
#include "MetalFramebuffer.hpp"

namespace Graphics::Metal {

	class MetalFramebufferBuilder final : public JarFramebufferBuilder {
		public:
			MetalFramebufferBuilder() = default;

			~MetalFramebufferBuilder() override = default;

			MetalFramebufferBuilder* SetFramebufferExtent(uint32_t width, uint32_t height) override;

			MetalFramebufferBuilder* SetImageFormat(PixelFormat format) override;

			MetalFramebufferBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			MetalFramebufferBuilder* SetImageBuffers(std::vector<std::shared_ptr<JarImageBuffer>> imageBuffers) override;

			std::shared_ptr<JarFramebuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<uint32_t> m_width;
			std::optional<uint32_t> m_height;
			std::optional<PixelFormat> m_pixelFormat;
			std::vector<std::shared_ptr<MetalImageBuffer>> m_imageBuffers;
			std::optional<std::shared_ptr<MetalImageBuffer>> m_targetImageBuffer;
			std::optional<std::shared_ptr<MetalRenderPass>> m_renderPass;
	};
}

#endif //JAREP_METALFRAMEBUFFERBUILDER_HPP
