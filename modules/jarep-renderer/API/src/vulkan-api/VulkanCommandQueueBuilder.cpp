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
		poolInfo.queueFamilyIndex = vulkanDevice->getGraphicsFamilyIndex().value();

		VkCommandPool commandPool;
		if (vkCreateCommandPool(vulkanDevice->getLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
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

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkAllocateCommandBuffers(vulkanDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()) !=
		    VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffer");
		}

		for (int i = 0; i < bufferCount; i++) {
			VkSemaphore imageAvailableSemaphore;
			VkSemaphore renderFinishedSemaphore;
			VkFence inFlightFence;

			if (vkCreateSemaphore(vulkanDevice->getLogicalDevice(), &semaphoreCreateInfo, nullptr,
			                      &imageAvailableSemaphore) != VK_SUCCESS ||
			    vkCreateSemaphore(vulkanDevice->getLogicalDevice(), &semaphoreCreateInfo, nullptr,
			                      &renderFinishedSemaphore) != VK_SUCCESS ||
			    vkCreateFence(vulkanDevice->getLogicalDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
				throw std::runtime_error("failed to create semaphores!");
			}

			vkCommandBuffers.push_back(new VulkanCommandBuffer(commandBuffers[i], imageAvailableSemaphore,
			                                                   renderFinishedSemaphore, inFlightFence));
		}

		// Create CommandQueue object
		return std::make_shared<VulkanCommandQueue>(vulkanDevice, commandPool, vkCommandBuffers);
	}
}