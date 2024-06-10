//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANIMAGEBUILDER_HPP
#define JAREP_VULKANIMAGEBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanImage.hpp"
#include "VulkanCommandQueue.hpp"
#include "VulkanDataTypeMaps.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>
#include <functional>

namespace Graphics::Vulkan {
	class VulkanCommandQueue;

	class VulkanDevice;

	class VulkanImageBuilder final : public JarImageBuilder {
		public:
			VulkanImageBuilder(std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCb)
					: createCmdQueueCallback(createCmdQueueCb) {};

			~VulkanImageBuilder() override;

			VulkanImageBuilder* SetPixelFormat(PixelFormat format) override;

			VulkanImageBuilder* SetImagePath(std::string imagePath) override;

			VulkanImageBuilder* EnableMipMaps(bool enabled) override;

			std::shared_ptr<JarImage> Build(std::shared_ptr<JarDevice> device) override;

			static void createImage(std::shared_ptr<VulkanDevice>& vulkanDevice, uint32_t texWidth, uint32_t texHeight,
			                        uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
			                        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			                        VkImage& image, VkDeviceMemory& imageMemory);

			static void createImageView(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat format,
			                            VkImageAspectFlags aspectFlags, VkImageView* view, uint32_t mipLevels);

		private:
			std::optional<PixelFormat> m_pixelFormat;
			std::optional<std::string> m_imagePath;
			bool m_enableMipMapping;
			static inline uint32_t m_nextImageId = 0;
			std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCallback;


			void
			transitionImageLayout(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat format,
			                      VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

			void copyBufferToImage(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer buffer, VkImage image,
			                       uint32_t width, uint32_t height);

			void generateMipMaps(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat imageFormat,
			                     int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

			static void
			createSampler(std::shared_ptr<VulkanDevice>& vulkanDevice, VkSampler& sampler, uint32_t mipLevels);
	};
}
#endif //JAREP_VULKANIMAGEBUILDER_HPP
