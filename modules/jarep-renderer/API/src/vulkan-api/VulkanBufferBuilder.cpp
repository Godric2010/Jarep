//
// Created by sebastian on 09.06.24.
//

#include "VulkanBufferBuilder.hpp"

namespace Graphics::Vulkan {
	static std::unordered_map<BufferUsage, VkBufferUsageFlags> bufferUsageMap{
			{BufferUsage::VertexBuffer,  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT},
			{BufferUsage::IndexBuffer,   VK_BUFFER_USAGE_INDEX_BUFFER_BIT},
			{BufferUsage::UniformBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},
			{BufferUsage::StoreBuffer,   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},
			{BufferUsage::TransferSrc,   VK_BUFFER_USAGE_TRANSFER_SRC_BIT},
			{BufferUsage::TransferDest,  VK_BUFFER_USAGE_TRANSFER_DST_BIT},
	};

	VulkanBufferBuilder::~VulkanBufferBuilder() = default;

	VulkanBufferBuilder* VulkanBufferBuilder::SetUsageFlags(Graphics::BufferUsage usageFlags) {
		m_bufferUsageFlags = std::make_optional<VkBufferUsageFlags>(bufferUsageMap[usageFlags]);
		return this;
	}

	VulkanBufferBuilder* VulkanBufferBuilder::SetMemoryProperties(Graphics::MemoryProperties memProps) {

		VkMemoryPropertyFlags vkFlags = 0;

		if (memProps.flags & MemoryProperties::DeviceLocal) vkFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (memProps.flags & MemoryProperties::HostVisible) vkFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		if (memProps.flags & MemoryProperties::HostCoherent) vkFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		if (memProps.flags & MemoryProperties::HostCached) vkFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		if (memProps.flags & MemoryProperties::LazilyAllocation) vkFlags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

		m_memoryPropertiesFlags = std::make_optional(vkFlags);
		return this;
	}

	VulkanBufferBuilder* VulkanBufferBuilder::SetBufferData(const void* data, size_t bufferSize) {
		m_bufferSize = std::make_optional<size_t>(bufferSize);
		m_data = std::make_optional(data);
		return this;
	}

	std::shared_ptr<JarBuffer> VulkanBufferBuilder::Build(std::shared_ptr<JarDevice> device) {

		if (!m_bufferSize.has_value() || !m_data.has_value() || !m_memoryPropertiesFlags.has_value() ||
		    !m_bufferUsageFlags.has_value())
			throw std::runtime_error("Buffer not correctly initialized! All fields must be set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		if (m_bufferUsageFlags.value() == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
			return BuildUniformBuffer(vulkanDevice);
		}

		m_onRegisterBuffer();
		auto size = m_bufferSize.value();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vulkanDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
		             stagingBufferMemory);

		void* stagingData;
		void* data = const_cast<void*>(m_data.value());
		vkMapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, 0, size, 0, &stagingData);
		memcpy(stagingData, data, size);
		vkUnmapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory);


		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		createBuffer(vulkanDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | m_bufferUsageFlags.value(),
		             m_memoryPropertiesFlags.value(), buffer, bufferMemory);

		copyBuffer(vulkanDevice, stagingBuffer, buffer, size);

		vkDestroyBuffer(vulkanDevice->getLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, nullptr);

		auto releasedCallback = [this]() { m_onDestroyBuffer(); };
		return std::make_shared<VulkanBuffer>(vulkanDevice, nextBufferId++, buffer, m_bufferSize.value(), bufferMemory,
		                                      data,
		                                      releasedCallback);
	}

	void VulkanBufferBuilder::createBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkDeviceSize size,
	                                       VkBufferUsageFlags usage,
	                                       VkMemoryPropertyFlags properties, VkBuffer& buffer,
	                                       VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vulkanDevice->getLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vertex buffer!");
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(vulkanDevice->getLogicalDevice(), buffer, &memoryRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(vulkanDevice->getPhysicalDevice(), memoryRequirements.memoryTypeBits,
		                                           properties);


		if (vkAllocateMemory(vulkanDevice->getLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(vulkanDevice->getLogicalDevice(), buffer, bufferMemory, 0);
	}

	void
	VulkanBufferBuilder::copyBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer srcBuffer, VkBuffer dstBuffer,
	                                VkDeviceSize size) {

		auto vulkanCommandPool = m_createCommandQueue();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(vulkanDevice, vulkanCommandPool);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		VulkanCommandBuffer::EndSingleTimeRecording(vulkanDevice, commandBuffer, vulkanCommandPool);

	}

	uint32_t VulkanBufferBuilder::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
	                                             VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			if (typeFilter & 1 << i &&
			    (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type!");
	}

	std::shared_ptr<JarBuffer> VulkanBufferBuilder::BuildUniformBuffer(std::shared_ptr<VulkanDevice> vulkanDevice) {

		VkBuffer buffer;
		VkDeviceMemory bufferMemory;


		createBuffer(vulkanDevice, m_bufferSize.value(), m_bufferUsageFlags.value(), m_memoryPropertiesFlags.value(),
		             buffer, bufferMemory);
		void* data;
		vkMapMemory(vulkanDevice->getLogicalDevice(), bufferMemory, 0, m_bufferSize.value(), 0, &data);

		auto releasedCallback = []() {};
		return std::make_shared<VulkanBuffer>(vulkanDevice, nextBufferId++, buffer, m_bufferSize.value(), bufferMemory,
		                                      data,
		                                      releasedCallback);
	}
}