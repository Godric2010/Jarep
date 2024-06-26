//
// Created by sebastian on 15.06.24.
//

#include "VulkanImageBuffer.hpp"

namespace Graphics::Vulkan {

	VulkanImageBuffer::VulkanImageBuffer(std::shared_ptr<VulkanDevice> device,
	                                     std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCb,
	                                     VkExtent2D imageExtent,
	                                     VkFormat pixelFormat,
	                                     uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkImageTiling tiling,
	                                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
	                                     VkImageAspectFlags aspectFlags) : m_device(std::move(device)), m_imageExtent(imageExtent), m_imageFormat(pixelFormat), m_mipLevels(mipLevels),
	                                                                       m_createCmdQueueCallback(createCmdQueueCb) {
		m_image = VK_NULL_HANDLE;
		m_imageMemory = VK_NULL_HANDLE;
		m_imageView = VK_NULL_HANDLE;

		CreateImage(numSamples, tiling, usage, properties);
		CreateImageView(aspectFlags);
	}

	void VulkanImageBuffer::UploadData(const void* data, size_t deviceSize) {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanBufferBuilder::CreateBuffer(m_device, deviceSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		                                  stagingBuffer, stagingBufferMemory);

		void* mappedData;
		vkMapMemory(m_device->GetLogicalDevice(), stagingBufferMemory, 0, deviceSize, 0, &mappedData);
		memcpy(mappedData, data, deviceSize);
		vkUnmapMemory(m_device->GetLogicalDevice(), stagingBufferMemory);

		TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer);
		TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(m_device->GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), stagingBufferMemory, nullptr);
	}

	void VulkanImageBuffer::Release() {
		vkDestroyImageView(m_device->GetLogicalDevice(), m_imageView, nullptr);
		vkDestroyImage(m_device->GetLogicalDevice(), m_image, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_imageMemory, nullptr);
	}

	void VulkanImageBuffer::CreateImage(VkSampleCountFlagBits numSamples, VkImageTiling tiling,
	                                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_imageExtent.width;
		imageInfo.extent.height = m_imageExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = m_mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = m_imageFormat;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;
		imageInfo.flags = 0;

		if (vkCreateImage(m_device->GetLogicalDevice(), &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device->GetLogicalDevice(), m_image, &memRequirements);
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanBufferBuilder::FindMemoryType(m_device->GetPhysicalDevice(),
		                                                                memRequirements.memoryTypeBits,
		                                                                properties);
		if (vkAllocateMemory(m_device->GetLogicalDevice(), &allocInfo, nullptr, &m_imageMemory) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate image memory!");
		}
		vkBindImageMemory(m_device->GetLogicalDevice(), m_image, m_imageMemory, 0);
	}


	void VulkanImageBuffer::CreateImageView(VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_imageFormat;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = m_mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_device->GetLogicalDevice(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture image view!");
		}
	}

	void VulkanImageBuffer::CopyBufferToImage(VkBuffer stagingBuffer) {
		auto commandQueue = m_createCmdQueueCallback();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(m_device, commandQueue);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {m_imageExtent.width, m_imageExtent.height, 1};

		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		VulkanCommandBuffer::EndSingleTimeRecording(m_device, commandBuffer, commandQueue);
	}

	void
	VulkanImageBuffer::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
		auto commandQueue = m_createCmdQueueCallback();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(m_device, commandQueue);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = m_mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationFlags;
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		           newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationFlags, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		VulkanCommandBuffer::EndSingleTimeRecording(m_device, commandBuffer, commandQueue);
	}

	VkImage VulkanImageBuffer::GetImage() { return m_image; }
}// namespace Graphics::Vulkan