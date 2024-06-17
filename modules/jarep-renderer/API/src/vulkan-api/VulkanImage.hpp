//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANIMAGE_HPP
#define JAREP_VULKANIMAGE_HPP

#include "IRenderAPI.hpp"
#include <utility>
#include <vulkan/vulkan.hpp>

#include "VulkanImageBuffer.hpp"
#include "VulkanBackend.hpp"

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanImageBuffer;

	class VulkanImage final : public JarImage {
		public:
			VulkanImage(std::shared_ptr<VulkanDevice> device, std::unique_ptr<VulkanImageBuffer> imageBuffer,
			            VkSampler sampler, uint32_t imageId) : m_device(std::move(device)),
			                                                   m_imageBuffer(std::move(imageBuffer)),
			                                                   m_sampler(sampler), m_imageId(imageId) {};

			~VulkanImage() override;

			void Release() override;

			[[nodiscard]] const VulkanImageBuffer& GetImageBuffer() const { return *m_imageBuffer; }

			[[nodiscard]] VkSampler GetSampler() const { return m_sampler; }

			[[nodiscard]] uint32_t GetImageId() const { return m_imageId; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			std::unique_ptr<VulkanImageBuffer> m_imageBuffer;
			VkSampler m_sampler;
			uint32_t m_imageId;
	};
}
#endif //JAREP_VULKANIMAGE_HPP
