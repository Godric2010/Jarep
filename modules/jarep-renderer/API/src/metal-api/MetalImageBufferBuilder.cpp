//
// Created by Sebastian Borsch on 10.07.24.
//

#include "MetalImageBufferBuilder.hpp"

namespace Graphics::Metal {

	MetalImageBufferBuilder::~MetalImageBufferBuilder() = default;

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetImageBufferExtent(uint32_t width, uint32_t height) {
		m_width = std::make_optional(width);
		m_height = std::make_optional(height);
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetImageFormat(PixelFormat format) {
		m_format = std::make_optional(pixelFormatMap[format]);
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetMipLevels(uint32_t mipLevels) {
		m_mipLevels = std::make_optional(mipLevels);
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetSampleCount(uint16_t sampleCount) {
		m_sampleCount = std::make_optional(sampleCount);
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetMemoryProperties(MemoryProperties memoryProperties) {
		m_memoryProperties = std::make_optional(TranslateMemoryPropertiesToMetal(memoryProperties));
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetImageTiling(ImageTiling tiling) {
		// The concept of tiling is not present in Metal API
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetImageUsage(ImageUsage usage) {
		m_usage = std::make_optional(TranslateImageUsageToMetal(usage));
		return this;
	}

	MetalImageBufferBuilder* MetalImageBufferBuilder::SetImageAspect(ImageAspect aspect) {
		// The concept of aspect is not present in Metal API
		return this;
	}

	std::shared_ptr<JarImageBuffer>
	MetalImageBufferBuilder::Build(std::shared_ptr<Backend> backend, std::shared_ptr<JarDevice> device) {

		if (!isBuildConditionSatisfied())
			throw std::runtime_error("Could not create image buffer! Not all required parameters are set!");

		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		auto metalBackend = reinterpret_cast<std::shared_ptr<MetalBackend>&>(backend);

		MTL::TextureType textureType;
		if (m_sampleCount.value() > 1 && !hasDepthOrStencilFormat()) {
			textureType = MTL::TextureType2DMultisample;
		} else {
			textureType = MTL::TextureType2D;
		}

		MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::alloc()->init();
		descriptor->setTextureType(textureType);
		descriptor->setWidth(m_width.value());
		descriptor->setHeight(m_height.value());
		descriptor->setPixelFormat(m_format.value());
		descriptor->setMipmapLevelCount(m_mipLevels.value());
		descriptor->setSampleCount(m_sampleCount.value());
		descriptor->setResourceOptions(m_memoryProperties.value());
		descriptor->setUsage(m_usage.value());

		MTL::Texture* texture = metalDevice->getDevice()->newTexture(descriptor);

		return std::make_shared<MetalImageBuffer>(metalDevice, texture, [metalBackend, metalDevice]() {
			auto jarQueue = metalBackend->InitCommandQueueBuilder()->SetCommandBufferAmount(2)->Build(metalDevice);
			return reinterpret_cast<std::shared_ptr<MetalCommandQueue>&>(jarQueue);
		});
	}

	bool MetalImageBufferBuilder::isBuildConditionSatisfied() {
		return m_width.has_value() && m_height.has_value() && m_format.has_value() && m_mipLevels.has_value() &&
		       m_sampleCount.has_value() && m_memoryProperties.has_value() && m_usage.has_value();
	}

	bool MetalImageBufferBuilder::hasDepthOrStencilFormat() {
		return m_format.value() == MTL::PixelFormatDepth32Float ||
		       m_format.value() == MTL::PixelFormatDepth32Float_Stencil8 ||
		       m_format.value() == MTL::PixelFormatDepth16Unorm ||
			   m_format.value() == MTL::PixelFormatDepth24Unorm_Stencil8||
			   m_format.value() == MTL::PixelFormatStencil8;
	}
}