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

		private:
			std::optional<PixelFormat> m_pixelFormat;
			std::optional<std::string> m_imagePath;
			bool m_enableMipMapping;
			static inline uint32_t m_nextImageId = 0;
			std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCallback;

			void generateMipMaps(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat imageFormat,
			                     int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

			static void
			createSampler(std::shared_ptr<VulkanDevice>& vulkanDevice, VkSampler& sampler, uint32_t mipLevels);
	};
}
#endif //JAREP_VULKANIMAGEBUILDER_HPP
