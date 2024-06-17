//
// Created by sebastian on 09.06.24.
//

#include "VulkanImageBuilder.hpp"

namespace Graphics::Vulkan {
	VulkanImageBuilder::~VulkanImageBuilder() = default;

	VulkanImageBuilder* VulkanImageBuilder::SetPixelFormat(Graphics::PixelFormat format) {
		m_pixelFormat = std::make_optional(format);
		return this;
	}

	VulkanImageBuilder* VulkanImageBuilder::SetImagePath(std::string imagePath) {
		m_imagePath = std::make_optional(imagePath);
		return this;
	}

	VulkanImageBuilder* VulkanImageBuilder::EnableMipMaps(bool enabled) {
		m_enableMipMapping = enabled;
		return this;
	}

	std::shared_ptr<JarImage> VulkanImageBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(m_imagePath.value().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		if (!pixels) {
			throw std::runtime_error("Failed to load texture image!");
		}


		auto mipLevels = 1;
		if (m_enableMipMapping) { mipLevels = (std::floor(std::log2(std::max(texWidth, texHeight)))) + 1; }
		auto imageExtent = VkExtent2D{static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)};

		std::unique_ptr<VulkanImageBuffer> imageBuffer = std::make_unique<VulkanImageBuffer>(vulkanDevice,
		                                                                                     createCmdQueueCallback,
		                                                                                     imageExtent,
		                                                                                     pixelFormatMap[m_pixelFormat.value()],
		                                                                                     mipLevels,
		                                                                                     VK_SAMPLE_COUNT_1_BIT,
		                                                                                     VK_IMAGE_TILING_OPTIMAL,
		                                                                                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		                                                                                     VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		                                                                                     VK_IMAGE_USAGE_SAMPLED_BIT,
		                                                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                                                                                     VK_IMAGE_ASPECT_COLOR_BIT);
		imageBuffer->UploadData(pixels, imageSize);

//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		VulkanBufferBuilder::createBuffer(vulkanDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//		                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//		                                  stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
//		memcpy(data, pixels, static_cast<size_t>(imageSize));
//		vkUnmapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory);
//
//		stbi_image_free(pixels);
//
//		auto vkFormat = pixelFormatMap[m_pixelFormat.value()];
//
//		VkImage textureImage;
//		VkDeviceMemory textureImageMemory;
//		createImage(vulkanDevice, texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, vkFormat,
//		            VK_IMAGE_TILING_OPTIMAL,
//		            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
//
//		transitionImageLayout(vulkanDevice, textureImage, vkFormat,
//		                      VK_IMAGE_LAYOUT_UNDEFINED,
//		                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
//		copyBufferToImage(vulkanDevice, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth),
//		                  static_cast<uint32_t>(texHeight));
//
//
//		//transitionImageLayout(vulkanDevice, textureImage, vkFormat,
//		//VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//		//		mipLevels);
//
//		vkDestroyBuffer(vulkanDevice->getLogicalDevice(), stagingBuffer, nullptr);
//		vkFreeMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, nullptr);

		generateMipMaps(vulkanDevice, imageBuffer->GetImage(), imageBuffer->GetFormat(), imageExtent.width,
		                imageExtent.height, mipLevels);


//		VkImageView textureImageView;
//		createImageView(vulkanDevice, textureImage, vkFormat, VK_IMAGE_ASPECT_COLOR_BIT, &textureImageView, mipLevels);
//
		VkSampler sampler;
		createSampler(vulkanDevice, sampler, mipLevels);

		return std::make_shared<VulkanImage>(vulkanDevice, std::move(imageBuffer), sampler, m_nextImageId++);
	}

	void VulkanImageBuilder::generateMipMaps(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image,
	                                         VkFormat imageFormat, int32_t texWidth, int32_t texHeight,
	                                         uint32_t mipLevels) {

		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(vulkanDevice->getPhysicalDevice(), imageFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("Texture image format does not support linear blitting!");
		}

		auto commandQueue = createCmdQueueCallback();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(vulkanDevice, commandQueue);

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
			                     nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = {0, 0, 0};
			blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = {0, 0, 0};
			blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
			               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			                     0, 0, nullptr, 0, nullptr, 1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		                     0, nullptr, 0, nullptr, 1, &barrier);

		VulkanCommandBuffer::EndSingleTimeRecording(vulkanDevice, commandBuffer, commandQueue);
	}

	void VulkanImageBuilder::createSampler(std::shared_ptr<VulkanDevice>& vulkanDevice, VkSampler& sampler,
	                                       uint32_t mipLevels) {

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(vulkanDevice->getPhysicalDevice(), &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);

		if (vkCreateSampler(vulkanDevice->getLogicalDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture sampler!");
		}
	}
}