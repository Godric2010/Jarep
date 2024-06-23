//
// Created by sebastian on 21.06.24.
//

#ifndef JAREP_VULKANIMAGEBUFFERBUILDER_HPP
#define JAREP_VULKANIMAGEBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanImageBuffer.hpp"
#include "VulkanDataTypeMaps.hpp"
#include <vulkan/vulkan.hpp>
#include <functional>
#include <optional>

namespace Graphics::Vulkan {
	class VulkanImageBufferBuilder final : public JarImageBufferBuilder {
		public:
			VulkanImageBufferBuilder() = default;

			~VulkanImageBufferBuilder() override = default;

			VulkanImageBufferBuilder* SetImageBufferExtent(uint32_t width, uint32_t height) override;

			VulkanImageBufferBuilder* SetImageFormat(PixelFormat format) override;

			VulkanImageBufferBuilder* SetMipLevels(uint32_t mipLevels) override;

			VulkanImageBufferBuilder* SetSampleCount(uint16_t sampleCount) override;

			VulkanImageBufferBuilder* SetImageTiling(ImageTiling imageTiling) override;

			VulkanImageBufferBuilder* SetImageUsage(ImageUsage imageUsage) override;

			VulkanImageBufferBuilder* SetImageAspect(ImageAspect imageAspect) override;

			VulkanImageBufferBuilder* SetMemoryProperties(MemoryProperties memoryProperties) override;

			std::unique_ptr<JarImageBuffer>
			Build(std::shared_ptr<Backend> backend, std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<VkExtent2D> m_imageExtent;
			std::optional<VkFormat> m_pixelFormat;
			std::optional<uint32_t> m_mipLevels;
			std::optional<VkSampleCountFlagBits> m_numSamples;
			std::optional<VkImageTiling> m_tiling;
			std::optional<VkImageUsageFlags> m_usage;
			std::optional<VkMemoryPropertyFlags> m_properties;
			std::optional<VkImageAspectFlags> m_aspectFlags;

	};
}

#endif //JAREP_VULKANIMAGEBUFFERBUILDER_HPP
