//
// Created by Godri on 3/27/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Rendering {
	struct SwapchainConfig {
		uint32_t width;
		uint32_t height;
		bool vsync = true;
		VkFormat preferredFormat = VK_FORMAT_B8G8R8A8_SRGB;
		VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	};

	/**
	 * @class VulkanSwapchain
	 * @brief Manages the Vulkan swapchain lifecycle, including creating, image view setup, and resizing.
	 *
	 * The VulkanSwapchain class encapsulates the creation and maintenance of the VkSwapchainKHR object,
	 * which is responsible for presenting rendered images to a window surface. It provides mechanisms
	 * to query device support, select optimal surface formats and present modes, and create image views
	 * for framebuffer attachments. The swapchain can be recreated on window resize or configuration changes.
	 */
	class VulkanSwapchain {
		public:
			VulkanSwapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
			                const SwapchainConfig&config);

			~VulkanSwapchain();

			void recreate(uint32_t width, uint32_t height);

			void cleanup(bool recreate = false);

			VkSwapchainKHR get() const;

			const std::vector<VkImageView>& getImageViews() const;

			VkFormat getFormat() const;

			VkExtent2D getExtent() const;

		private:
			void createSwapchain();

			void createImageViews();

			void querySupportDetails();

			void chooseSurfaceFormat();

			void choosePresentMode();

			void chooseExtent();

			VkDevice m_device;
			VkPhysicalDevice m_physicalDevice;
			VkSurfaceKHR m_surface;
			SwapchainConfig m_config;

			VkSwapchainKHR m_swapchain;
			VkFormat m_format;
			VkExtent2D m_extent;
			std::vector<VkImage> m_images;
			std::vector<VkImageView> m_imageViews;

			VkSurfaceFormatKHR m_surfaceFormat;
			VkPresentModeKHR m_presentMode;
			VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	};
}
