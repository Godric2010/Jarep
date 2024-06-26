//
// Created by sebastian on 09.06.24.
//

#include "VulkanSwapchain.hpp"

namespace Graphics::Vulkan {
	void VulkanSwapchain::Initialize(VkExtent2D extent, SwapChainSupportDetails swapchainSupport) {
		m_swapchainSupport = std::move(swapchainSupport);
		m_imageExtent = extent;

		m_graphicsQueue = m_device->GetGraphicsQueue().value();
		m_presentQueue = m_device->GetPresentQueue().value();

		//		vkGetDeviceQueue(m_device->GetLogicalDevice(), m_device->GetGraphicsFamilyIndex().value(), 0, &m_graphicsQueue);
		//		vkGetDeviceQueue(m_device->GetLogicalDevice(), m_device->GetPresentFamilyIndex().value(), 0, &m_presentQueue);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(m_swapchainSupport.formats);
		const VkPresentModeKHR presentMode = chooseSwapPresentMode(m_swapchainSupport.presentModes);
		m_imageExtent = chooseSwapExtent(m_swapchainSupport.capabilities, m_imageExtent);

		m_swapchainImageFormat = surfaceFormat.format;

		m_swapchainMaxImageCount = m_swapchainSupport.capabilities.minImageCount + 1;
		if (m_swapchainSupport.capabilities.maxImageCount > 0 &&
		    m_swapchainMaxImageCount > m_swapchainSupport.capabilities.maxImageCount) {
			m_swapchainMaxImageCount = m_swapchainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = m_swapchainMaxImageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = m_imageExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		const uint32_t queueFamilyIndices[] = {
		        m_device->GetGraphicsFamilyIndex().value(), m_device->GetPresentFamilyIndex().value()};
		if (m_device->GetGraphicsFamilyIndex() != m_device->GetPresentFamilyIndex()) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = m_swapchainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_device->GetLogicalDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_device->GetLogicalDevice(), m_swapchain, &m_swapchainMaxImageCount, nullptr);
		m_swapchainImages.resize(m_swapchainMaxImageCount);
		vkGetSwapchainImagesKHR(m_device->GetLogicalDevice(), m_swapchain, &m_swapchainMaxImageCount,
		                        m_swapchainImages.data());

		createImageViews();
		m_currentImageIndex = 0;
	}

	std::optional<uint32_t> VulkanSwapchain::AcquireNewImage(VkSemaphore imageAvailable,
	                                                         VkFence frameInFlight) {
		vkWaitForFences(m_device->GetLogicalDevice(), 1, &frameInFlight, VK_TRUE, UINT64_MAX);


		auto result = vkAcquireNextImageKHR(m_device->GetLogicalDevice(), m_swapchain, UINT64_MAX, imageAvailable,
		                                    VK_NULL_HANDLE,
		                                    &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			std::cout << " swapchain out of date" << std::endl;
			return std::nullopt;
		}
		vkResetFences(m_device->GetLogicalDevice(), 1, &frameInFlight);
		return std::make_optional(m_currentImageIndex);
	}

	void VulkanSwapchain::PresentImage(VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence frameInFlight,
	                                   VkCommandBuffer* cmdBuffer) {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {imageAvailable};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdBuffer;
		VkSemaphore signalSemaphores[] = {renderFinished};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		auto result = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, frameInFlight);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer");
		}

		const uint32_t currentImageIndex = static_cast<uint32_t>(m_currentImageIndex);
		const uint32_t* currentImageIndexPtr = &currentImageIndex;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		const VkSwapchainKHR swapchains[] = {m_swapchain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = currentImageIndexPtr;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(m_presentQueue, &presentInfo);

		m_currentImageIndex = (m_currentImageIndex + 1) % m_swapchainMaxImageCount;
	}

	void VulkanSwapchain::RecreateSwapchain(uint32_t width, uint32_t height, SwapChainSupportDetails swapchainSupport) {
		Release();
		m_imageExtent.width = width;
		m_imageExtent.height = height;
		Initialize(m_imageExtent, std::move(swapchainSupport));
	}

	void VulkanSwapchain::Release() {

		vkQueueWaitIdle(m_graphicsQueue);
		vkQueueWaitIdle(m_presentQueue);


		for (const auto imageView: m_swapchainImageViews) {
			vkDestroyImageView(m_device->GetLogicalDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_device->GetLogicalDevice(), m_swapchain, nullptr);
	}


	VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(
	        const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat: availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			    availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(
	        const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode: availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
	                                             const VkExtent2D surfaceExtent) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D actualExtent;
			actualExtent.width = std::clamp(surfaceExtent.width, capabilities.minImageExtent.width,
			                                capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(surfaceExtent.height, capabilities.minImageExtent.height,
			                                 capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void VulkanSwapchain::createImageViews() {
		m_swapchainImageViews.resize(m_swapchainImages.size());
		for (size_t i = 0; i < m_swapchainImages.size(); i++) {
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_swapchainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_swapchainImageFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->GetLogicalDevice(), &viewInfo, nullptr, &m_swapchainImageViews[i]) !=
			    VK_SUCCESS) {
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}
}// namespace Graphics::Vulkan