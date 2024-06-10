//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANBUFFER_HPP
#define JAREP_VULKANBUFFER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanBuffer final : public JarBuffer {
		public:
			VulkanBuffer(std::shared_ptr<VulkanDevice>& device, uint32_t bufferId, VkBuffer buffer, size_t bufferSize,
			             VkDeviceMemory deviceMemory,
			             void* data, const std::function<void()>& bufferReleasedCallback)
					: m_device(device), m_buffer(buffer), m_bufferSize(bufferSize), m_bufferMemory(deviceMemory),
					  m_data(data),
					  m_bufferReleasedCallback(bufferReleasedCallback), id(bufferId) {

			};

			~VulkanBuffer() override;

			void Release() override;

			void Update(const void* data, size_t bufferSize) override;

			[[nodiscard]] VkBuffer getBuffer() const { return m_buffer; }

			[[nodiscard]] size_t getBufferSize() const { return m_bufferSize; }

			uint32_t getBufferId() const { return id; }

		private:
			// Buffer ID management
			uint32_t id;

			// Buffer data
			VkBuffer m_buffer;
			VkDeviceMemory m_bufferMemory;
			size_t m_bufferSize;
			void* m_data;
			std::shared_ptr<VulkanDevice> m_device;

			// Buffer callbacks
			std::function<void()> m_bufferReleasedCallback;
	};
}

#endif //JAREP_VULKANBUFFER_HPP
