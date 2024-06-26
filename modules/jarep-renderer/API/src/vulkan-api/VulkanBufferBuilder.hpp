//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANBUFFERBUILDER_HPP
#define JAREP_VULKANBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandQueue.hpp"
#include <utility>
#include <vulkan/vulkan.hpp>
#include <optional>
#include <memory>
#include <functional>

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanBuffer;

	class VulkanCommandQueue;

	class VulkanBufferBuilder final : public JarBufferBuilder {

		public:
			VulkanBufferBuilder(std::function<std::shared_ptr<VulkanCommandQueue>()> createCommandQueueCb, std::function<void()> onRegister, std::function<void()> onDestroy ) : m_createCommandQueue(
					createCommandQueueCb), m_onRegisterBuffer(onRegister), m_onDestroyBuffer(onDestroy) {}

			~VulkanBufferBuilder() override;

			VulkanBufferBuilder* SetUsageFlags(BufferUsage usageFlags) override;

			VulkanBufferBuilder* SetMemoryProperties(MemoryProperties memProps) override;

			VulkanBufferBuilder* SetBufferData(const void* data, size_t bufferSize) override;

			std::shared_ptr<JarBuffer> Build(std::shared_ptr<JarDevice> device) override;

			static void
		    CreateBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkDeviceSize size, VkBufferUsageFlags usage,
			             VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

			static uint32_t
		    FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		private:

			std::function<std::shared_ptr<VulkanCommandQueue>()> m_createCommandQueue;
			std::function<void()> m_onRegisterBuffer;
			std::function<void()> m_onDestroyBuffer;
			std::optional<VkBufferUsageFlags> m_bufferUsageFlags;
			std::optional<VkMemoryPropertyFlags> m_memoryPropertiesFlags;
			std::optional<size_t> m_bufferSize;
			std::optional<const void*> m_data;
			static inline uint32_t nextBufferId = 0;

			void CopyBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer srcBuffer, VkBuffer dstBuffer,
			                VkDeviceSize size);

			std::shared_ptr<JarBuffer> BuildUniformBuffer(std::shared_ptr<VulkanDevice> vulkanDevice);

	};

}

#endif //JAREP_VULKANBUFFERBUILDER_HPP
