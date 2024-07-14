//
// Created by Sebastian Borsch on 10.07.24.
//

#ifndef JAREP_METALIMAGEBUFFERBUILDER_HPP
#define JAREP_METALIMAGEBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include "MetalImageBuffer.hpp"
#include "MetalDevice.hpp"
#include "MetalBackend.hpp"
#include "MetalDataTypeMaps.hpp"
#include <memory>
#include <optional>

namespace Graphics::Metal {
	class MetalImageBufferBuilder final : public JarImageBufferBuilder {

		public:
			MetalImageBufferBuilder() = default;

			~MetalImageBufferBuilder() override;

			MetalImageBufferBuilder* SetImageBufferExtent(uint32_t width, uint32_t height) override;

			MetalImageBufferBuilder* SetImageFormat(PixelFormat format) override;

			MetalImageBufferBuilder* SetMipLevels(uint32_t mipLevels) override;

			MetalImageBufferBuilder* SetSampleCount(uint16_t sampleCount) override;

			MetalImageBufferBuilder* SetMemoryProperties(MemoryProperties memoryProperties) override;

			MetalImageBufferBuilder* SetImageTiling(ImageTiling tiling) override;

			MetalImageBufferBuilder* SetImageUsage(ImageUsage usage) override;

			MetalImageBufferBuilder* SetImageAspect(ImageAspect aspect) override;

			std::shared_ptr<JarImageBuffer>
			Build(std::shared_ptr<Backend> backend, std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<uint32_t> m_width;
			std::optional<uint32_t> m_height;
			std::optional<MTL::PixelFormat> m_format;
			std::optional<uint32_t> m_mipLevels;
			std::optional<uint16_t> m_sampleCount;
			std::optional<MTL::ResourceOptions> m_memoryProperties;
			std::optional<MTL::TextureUsage> m_usage;

			bool isBuildConditionSatisfied();
			bool hasDepthOrStencilFormat();

	};
}


#endif //JAREP_METALIMAGEBUFFERBUILDER_HPP
