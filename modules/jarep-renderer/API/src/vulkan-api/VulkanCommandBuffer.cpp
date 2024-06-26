//
// Created by sebastian on 09.06.24.
//

#include "VulkanCommandBuffer.hpp"
namespace Graphics::Vulkan {
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, std::shared_ptr<VulkanDevice> device) {
		m_device = device;
		m_commandBuffer = commandBuffer;
		CreateSemaphore(m_imageAvailableSemaphore);
		CreateSemaphore(m_renderStepSemaphore);
		CreateSemaphore(m_renderFinishedSemaphore);
		CreateSemaphore(m_blitSemaphore);
		CreateFence(m_frameInFlightFence);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer() = default;

	void VulkanCommandBuffer::SetDepthBias(Graphics::DepthBias depthBias) {
		vkCmdSetDepthBias(m_commandBuffer, depthBias.DepthBiasConstantFactor, depthBias.DepthBiasClamp,
		                  depthBias.DepthBiasSlopeFactor);
	}

	void VulkanCommandBuffer::SetViewport(Graphics::Viewport viewport) {
		VkViewport vkViewport{};
		vkViewport.x = viewport.x;
		vkViewport.y = viewport.height;
		vkViewport.width = viewport.width;
		vkViewport.height = -viewport.height;
		vkViewport.minDepth = viewport.minDepth;
		vkViewport.maxDepth = viewport.maxDepth;
		vkCmdSetViewport(m_commandBuffer, 0, 1, &vkViewport);
	}

	void VulkanCommandBuffer::SetScissor(Graphics::Scissor scissor) {
		VkRect2D vkScissor{};
		vkScissor.offset = {scissor.x, scissor.y};
		vkScissor.extent = {scissor.width, scissor.height};
		vkCmdSetScissor(m_commandBuffer, 0, 1, &vkScissor);
	}

	bool VulkanCommandBuffer::StartRecording(std::shared_ptr<JarFramebuffer> framebuffer,
	                                         std::shared_ptr<JarRenderPass> renderPass) {
		auto vkRenderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);
		auto vulkanFramebuffer = reinterpret_cast<std::shared_ptr<VulkanFramebuffer>&>(framebuffer);

		vkResetCommandBuffer(m_commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vkRenderPass->GetRenderPass();
		renderPassInfo.framebuffer = vulkanFramebuffer->GetFramebuffer();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = vulkanFramebuffer->GetFramebufferExtent();
		renderPassInfo.clearValueCount = clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		return true;
	}

	void VulkanCommandBuffer::EndRecording() {
		vkCmdEndRenderPass(m_commandBuffer);
		if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_renderStepSemaphore;

		auto result = vkQueueSubmit(m_device->GetGraphicsQueue().value(), 1, &submitInfo, m_frameInFlightFence);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit commandbuffer on end recording");
		}

		vkWaitForFences(m_device->GetLogicalDevice(), 1, &m_frameInFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_device->GetLogicalDevice(), 1, &m_frameInFlightFence);
	}

	void VulkanCommandBuffer::Draw() {
		vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
	}

	void VulkanCommandBuffer::DrawIndexed(size_t indexAmount) {
		vkCmdDrawIndexed(m_commandBuffer, indexAmount, 1, 0, 0, 0);
	}

	void VulkanCommandBuffer::Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) {
		const auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);
		vkSurface->getSwapchain()->PresentImage(m_imageAvailableSemaphore, m_renderFinishedSemaphore,
		                                        m_frameInFlightFence, &m_commandBuffer);
	}

	void VulkanCommandBuffer::BindPipeline(std::shared_ptr<JarPipeline> pipeline, uint32_t frameIndex) {
		const auto vkPipeline = reinterpret_cast<std::shared_ptr<VulkanGraphicsPipeline>&>(pipeline);
		vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->getPipeline());
		m_pipeline = std::make_optional(vkPipeline);
	}

	void VulkanCommandBuffer::BindDescriptors(std::vector<std::shared_ptr<JarDescriptor>> descriptors) {

		if (!m_pipeline.has_value())
			throw std::runtime_error("Pipeline not bound");

		std::vector<VkDescriptorSet> descriptorSets;
		for (auto descriptor: descriptors) {
			const auto vkDescriptor = reinterpret_cast<std::shared_ptr<VulkanDescriptor>&>(descriptor);
			descriptorSets.push_back(vkDescriptor->GetNextDescriptorSet());
		}
		vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        m_pipeline.value()->getPipelineLayout(), 0,
		                        static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
	}

	void VulkanCommandBuffer::BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) {
		const auto vulkanBuffer = reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(buffer);
		const VkBuffer vertexBuffers[] = {vulkanBuffer->getBuffer()};
		const VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, vertexBuffers, offsets);
	}

	void VulkanCommandBuffer::BindIndexBuffer(std::shared_ptr<JarBuffer> indexBuffer) {
		const auto vulkanBuffer = reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(indexBuffer);
		vkCmdBindIndexBuffer(m_commandBuffer, vulkanBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
	}

	void VulkanCommandBuffer::Release(VkDevice device) {

		vkDestroySemaphore(device, m_imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(device, m_renderFinishedSemaphore, nullptr);
		vkDestroyFence(device, m_frameInFlightFence, nullptr);
	}

	void VulkanCommandBuffer::BlitFramebuffersToSurface(std::shared_ptr<JarSurface> surface, std::vector<std::shared_ptr<JarFramebuffer>> framebuffers) {

		auto vulkanSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_renderStepSemaphore;
		VkPipelineStageFlags waitStageFlags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.pWaitDstStageMask = waitStageFlags;

		auto currentFrameIndex = vulkanSurface->getSwapchain()->AcquireNewImage(m_imageAvailableSemaphore, m_frameInFlightFence);
		if (!currentFrameIndex.has_value()) {
			throw std::runtime_error("Failed to acquire swapchain image!");
		}

		VkImage swapchainImage = vulkanSurface->getSwapchain()->GetSwapchainImage(currentFrameIndex.value());
		vkResetCommandBuffer(m_commandBuffer, 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording blit command buffer");
		}

		TransitionImageLayout(swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		for (auto framebuffer: framebuffers) {

			auto vulkanFramebuffer = reinterpret_cast<std::shared_ptr<VulkanFramebuffer>&>(framebuffer);
			VkImage srcImage = vulkanFramebuffer->GetSrcImage();

			TransitionImageLayout(srcImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

			VkImageBlit blitInfo{};
			blitInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blitInfo.srcSubresource.mipLevel = 0;
			blitInfo.srcSubresource.baseArrayLayer = 0;
			blitInfo.srcSubresource.layerCount = 1;
			blitInfo.srcOffsets[0] = {0, 0, 0};
			blitInfo.srcOffsets[1] = {static_cast<int32_t>(vulkanFramebuffer->GetFramebufferExtent().width), static_cast<int32_t>(vulkanFramebuffer->GetFramebufferExtent().height), 1};

			blitInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blitInfo.dstSubresource.mipLevel = 0;
			blitInfo.dstSubresource.baseArrayLayer = 0;
			blitInfo.dstSubresource.layerCount = 1;
			blitInfo.dstOffsets[0] = {0, 0, 0};
			blitInfo.dstOffsets[1] = {static_cast<int32_t>(vulkanSurface->getSurfaceExtent().width), static_cast<int32_t>(vulkanSurface->getSurfaceExtent().height), 1};

			vkCmdBlitImage(m_commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VK_FILTER_LINEAR);

			TransitionImageLayout(srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
		TransitionImageLayout(swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_blitSemaphore;
	}

	void VulkanCommandBuffer::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		} else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(m_commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

	VkCommandBuffer VulkanCommandBuffer::StartSingleTimeRecording(std::shared_ptr<VulkanDevice>& device,
	                                                              std::shared_ptr<VulkanCommandQueue> commandQueue) {

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandQueue->GetCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device->GetLogicalDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void
	VulkanCommandBuffer::EndSingleTimeRecording(std::shared_ptr<VulkanDevice>& vulkanDevice,
	                                            VkCommandBuffer commandBuffer,
	                                            std::shared_ptr<VulkanCommandQueue>& commandQueue) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkQueue graphicsQueue;
		vkGetDeviceQueue(vulkanDevice->GetLogicalDevice(), vulkanDevice->GetGraphicsFamilyIndex().value(), 0,
		                 &graphicsQueue);
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(vulkanDevice->GetLogicalDevice(), commandQueue->GetCommandPool(), 1, &commandBuffer);
	}


	void VulkanCommandBuffer::CreateSemaphore(VkSemaphore& semaphore) {
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr,
		                      &semaphore) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create semaphore!");
		}
	}

	void VulkanCommandBuffer::CreateFence(VkFence& fence) {

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(m_device->GetLogicalDevice(), &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}
}// namespace Graphics::Vulkan