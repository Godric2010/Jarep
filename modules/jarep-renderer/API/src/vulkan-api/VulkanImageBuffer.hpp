//
// Created by sebastian on 15.06.24.
//
#pragma once
#ifndef JAREP_VULKANIMAGEBUFFER_HPP
#define JAREP_VULKANIMAGEBUFFER_HPP

#include "VulkanDevice.hpp"
#include "VulkanCommandQueue.hpp"
#include <functional>
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanDevice;

	class VulkanCommandQueue;

	class VulkanImageBuffer {

		public:
			VulkanImageBuffer(std::shared_ptr<VulkanDevice> device,
			                  std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCb,
			                  VkExtent2D imageExtent, VkFormat pixelFormat,
			                  uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkImageTiling tiling,
			                  VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			                  VkImageAspectFlags aspectFlags);

			~VulkanImageBuffer() = default;

			void UploadData(void* data, VkDeviceSize deviceSize);

			void Release();

			[[nodiscard]] const VkImage& GetImage() const { return m_image; }

			[[nodiscard]] const VkImageView& GetImageView() const { return m_imageView; }

			[[nodiscard]] const VkDeviceMemory& GetMemory() const { return m_imageMemory; }

			[[nodiscard]] const VkFormat& GetFormat() const { return m_imageFormat; }

			[[nodiscard]] const VkExtent2D& GetExtent() const { return m_imageExtent; }

		private:
			VkImage m_image;
			VkDeviceMemory m_imageMemory;
			VkImageView m_imageView;
			VkFormat m_imageFormat;
			VkExtent2D m_imageExtent;
			uint32_t m_mipLevels;
			std::shared_ptr<VulkanDevice> m_device;
			std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCallback;

			void createImage(VkSampleCountFlagBits numSamples, VkImageTiling tiling,
			                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

			void createImageView(VkImageAspectFlags aspectFlags);

			void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

			void copyBufferToImage(VkBuffer stagingBuffer);
	};
}

#endif //JAREP_VULKANIMAGEBUFFER_HPP
