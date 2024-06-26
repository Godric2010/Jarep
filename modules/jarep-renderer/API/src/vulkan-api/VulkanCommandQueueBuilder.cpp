//
// Created by sebastian on 09.06.24.
//

#include "VulkanCommandQueueBuilder.hpp"

namespace Graphics::Vulkan {
	VulkanCommandQueueBuilder::~VulkanCommandQueueBuilder() = default;

	VulkanCommandQueueBuilder* VulkanCommandQueueBuilder::SetCommandBufferAmount(uint32_t commandBufferAmount) {
		m_amountOfCommandBuffers = std::make_optional<uint32_t>(commandBufferAmount);
		return this;
	}

	std::shared_ptr<JarCommandQueue> VulkanCommandQueueBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		// Create command pool
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = vulkanDevice->GetGraphicsFamilyIndex().value();

		VkCommandPool commandPool;
		if (vkCreateCommandPool(vulkanDevice->GetLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool");
		}

		// Create buffers in pool
		uint32_t bufferCount = m_amountOfCommandBuffers.value_or(DEFAULT_COMMAND_BUFFER_COUNT);
		std::vector<VulkanCommandBuffer*> vkCommandBuffers = std::vector<VulkanCommandBuffer*>();
		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.resize(bufferCount);

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = (uint32_t) commandBuffers.size();

		if (vkAllocateCommandBuffers(vulkanDevice->GetLogicalDevice(), &allocateInfo, commandBuffers.data()) !=
		    VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffer");
		}

		for (int i = 0; i < bufferCount; i++) {
			vkCommandBuffers.push_back(new VulkanCommandBuffer(commandBuffers[i], vulkanDevice));
		}

		// Create CommandQueue object
		return std::make_shared<VulkanCommandQueue>(vulkanDevice, commandPool, vkCommandBuffers);
	}
}// namespace Graphics::Vulkan