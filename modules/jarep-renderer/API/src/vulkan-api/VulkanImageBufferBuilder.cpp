//
// Created by sebastian on 21.06.24.
//

#include "VulkanImageBufferBuilder.hpp"

namespace Graphics::Vulkan {

	VulkanImageBufferBuilder::~VulkanImageBufferBuilder() = default;

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetImageFormat(PixelFormat pixelFormat) {
		m_pixelFormat = std::make_optional(pixelFormatMap[pixelFormat]);
		return this;
	}

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetImageUsage(ImageUsage usage) {
		m_usage = std::make_optional(convertToVkImageUsageFlags(usage));
		return this;
	}


	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetImageAspect(ImageAspect aspect) {
		m_aspectFlags = std::make_optional(convertToVkImageAspectFlags(aspect));
		return this;
	}

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetImageTiling(ImageTiling tiling) {
		m_tiling = std::make_optional(imageTilingMap[tiling]);
		return this;
	}

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetImageBufferExtent(uint32_t width, uint32_t height) {
		m_imageExtent = std::make_optional(VkExtent2D{width, height});
		return this;
	}

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetMipLevels(uint32_t mipLevels) {
		m_mipLevels = std::make_optional(mipLevels);
		return this;
	}

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetSampleCount(uint16_t sampleCount) {
		m_numSamples = std::make_optional(convertToVkSampleCountFlagBits(sampleCount));
		return this;
	}

	VulkanImageBufferBuilder* VulkanImageBufferBuilder::SetMemoryProperties(MemoryProperties memoryProperties) {
		m_properties = std::make_optional(convertToVkMemoryPropertyFlags(memoryProperties));
		return this;
	}


	std::shared_ptr<JarImageBuffer> VulkanImageBufferBuilder::Build(std::shared_ptr<Backend> backend,
	                                                                std::shared_ptr<JarDevice> device) {
		if (!m_imageExtent.has_value() || !m_pixelFormat.has_value() || !m_mipLevels.has_value() ||
		    !m_numSamples.has_value() || !m_tiling.has_value() || !m_usage.has_value() || !m_aspectFlags.has_value() ||
		    !m_properties.has_value())
			throw std::runtime_error("VulkanImageBuffer has some uninitialized fields!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		auto vulkanBackend = reinterpret_cast<std::shared_ptr<VulkanBackend>&>(backend);

		return std::make_shared<VulkanImageBuffer>(vulkanDevice,
		                                           [vulkanBackend]() { return vulkanBackend->getStagingCommandQueue(); },
		                                           m_imageExtent.value(), m_pixelFormat.value(), m_mipLevels.value(),
		                                           m_numSamples.value(), m_tiling.value(), m_usage.value(),
		                                           m_properties.value(), m_aspectFlags.value());
	}
}