//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANSWAPCHAIN_HPP
#define JAREP_VULKANSWAPCHAIN_HPP

#include "VulkanDevice.hpp"
#include "VulkanImageBuilder.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanDevice;

	struct SwapChainSupportDetails {
		public:
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapchain {
		public:
			VulkanSwapchain(std::shared_ptr<VulkanDevice> device, VkSurfaceKHR surface
			) : m_device(std::move(device)), m_surface(surface) {};

			~VulkanSwapchain() = default;

			void Initialize(VkExtent2D extent, SwapChainSupportDetails swapchainSupport);

			std::optional<uint32_t> AcquireNewImage(VkSemaphore imageAvailable, VkFence frameInFlight);

			void PresentImage(VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence frameInFlight,
			                  VkCommandBuffer* cmdBuffer);

			void RecreateSwapchain(uint32_t width, uint32_t height, SwapChainSupportDetails swapchainSupport);

			void Release();

			[[nodiscard]] VkFormat getSwapchainImageFormat() const { return m_swapchainImageFormat; }

			[[nodiscard]] uint32_t getCurrentImageIndex() const { return m_currentImageIndex; }

			[[nodiscard]] uint32_t getMaxSwapchainImageCount() const { return m_swapchainMaxImageCount; }

			[[nodiscard]] VkExtent2D getSwapchainImageExtent() const { return m_imageExtent; }

			[[nodiscard]] std::vector<VkImageView> getSwapchainImageViews() const { return m_swapchainImageViews; }

		private:
			VkSurfaceKHR m_surface;
			SwapChainSupportDetails m_swapchainSupport;
			std::shared_ptr<VulkanDevice> m_device;

			VkExtent2D m_imageExtent;
			VkQueue m_graphicsQueue;
			VkQueue m_presentQueue;
			VkFormat m_swapchainImageFormat;
			VkSwapchainKHR m_swapchain;
			std::vector<VkImage> m_swapchainImages;
			std::vector<VkImageView> m_swapchainImageViews;

			uint32_t m_currentImageIndex;
			uint32_t m_swapchainMaxImageCount;

			[[nodiscard]] static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
			                                                 VkExtent2D surfaceExtent);

			static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

			static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

			void createImageViews();
	};
}

#endif //JAREP_VULKANSWAPCHAIN_HPP
