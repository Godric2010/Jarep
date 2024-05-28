//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalImageBuilder.hpp"

namespace Graphics::Metal {
	MetalImageBuilder::~MetalImageBuilder() =
	default;

	MetalImageBuilder* MetalImageBuilder::SetImagePath(std::string path) {
		m_imagePath = std::make_optional(path);
		return this;
	}

	MetalImageBuilder* MetalImageBuilder::SetPixelFormat(Graphics::PixelFormat format) {
		m_pixelFormat = std::make_optional(pixelFormatMap[format]);
		return this;
	}

	MetalImageBuilder* MetalImageBuilder::EnableMipMaps(bool enabled) {
		m_enableMipMapping = enabled;
		return this;
	}

	std::shared_ptr<JarImage> MetalImageBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		if (!m_imagePath.has_value() || !m_pixelFormat.has_value())
			throw std::runtime_error("Could not create image! Image path and/or format are undefined!");

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(m_imagePath.value().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
			throw std::runtime_error("Failed to load image from path: " + m_imagePath.value());

		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(m_pixelFormat.value());
		textureDescriptor->setWidth(width);
		textureDescriptor->setHeight(height);

		uint32_t mipLevels = 1;

		if (m_enableMipMapping) {
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
			textureDescriptor->setMipmapLevelCount(mipLevels);
		}

		MTL::Texture* texture = metalDevice->getDevice()->newTexture(textureDescriptor);

		MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
		NS::UInteger bytesPerRow = 4 * width;

		texture->replaceRegion(region, 0, data, bytesPerRow);

		generateMipMaps(metalDevice, texture);

		textureDescriptor->release();
		stbi_image_free(data);

		MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
		samplerDescriptor->setMinFilter(MTL::SamplerMinMagFilterLinear);
		samplerDescriptor->setMagFilter(MTL::SamplerMinMagFilterLinear);
		samplerDescriptor->setMipFilter(MTL::SamplerMipFilterLinear);
		samplerDescriptor->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
		samplerDescriptor->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
		samplerDescriptor->setLodMinClamp(static_cast<float>(0));
		samplerDescriptor->setLodMaxClamp(static_cast<float>(mipLevels + 1));

		MTL::SamplerState* sampler = metalDevice->getDevice()->newSamplerState(samplerDescriptor);
		samplerDescriptor->release();


		return std::make_shared<MetalImage>(texture, sampler);
	}

	void MetalImageBuilder::generateMipMaps(std::shared_ptr<MetalDevice>& device, MTL::Texture* texture) {
		auto queueBuilder = new MetalCommandQueueBuilder();
		auto queue = queueBuilder->SetCommandBufferAmount(1)->Build(device);

		auto commandBuffer = reinterpret_cast<MetalCommandBuffer*>(queue->getNextCommandBuffer());
		auto blitCommandEncoder = commandBuffer->getCommandBuffer()->blitCommandEncoder();
		blitCommandEncoder->generateMipmaps(texture);
		blitCommandEncoder->endEncoding();
		commandBuffer->getCommandBuffer()->addCompletedHandler([](MTL::CommandBuffer* buffer) {
			buffer->release();
		});
		commandBuffer->getCommandBuffer()->commit();

	}
}