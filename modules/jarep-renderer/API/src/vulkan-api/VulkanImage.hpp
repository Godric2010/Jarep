//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANIMAGE_HPP
#define JAREP_VULKANIMAGE_HPP

#include "IRenderAPI.hpp"
#include <vulkan/vulkan.hpp>

#include "VulkanBackend.hpp"

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanImage final : public JarImage {
		public:
			VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage image, VkDeviceMemory imageMemory,
			            VkImageView imageView, VkFormat format, VkExtent2D extent, VkSampler sampler, uint32_t imageId)
					: m_device(device), m_image(image), m_imageMemory(imageMemory), m_imageView(imageView),
					  m_format(format), m_extent(extent), m_sampler(sampler), m_imageId(imageId) {};

			~VulkanImage() override;

			void Release() override;

			[[nodiscard]] VkImage getImage() const { return m_image; }

			[[nodiscard]] VkDeviceMemory getImageMemory() const { return m_imageMemory; }

			[[nodiscard]] VkImageView getImageView() const { return m_imageView; }

			[[nodiscard]] VkFormat getFormat() const { return m_format; }

			[[nodiscard]] VkExtent2D getExtent() const { return m_extent; }

			[[nodiscard]] VkSampler getSampler() const { return m_sampler; }

			[[nodiscard]] uint32_t getImageId() const { return m_imageId; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkImage m_image;
			VkDeviceMemory m_imageMemory;
			VkImageView m_imageView;
			VkFormat m_format;
			VkExtent2D m_extent;
			VkSampler m_sampler;
			uint32_t m_imageId;
	};
}
#endif //JAREP_VULKANIMAGE_HPP
