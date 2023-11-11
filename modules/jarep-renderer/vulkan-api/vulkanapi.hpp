//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_VULKANAPI_HPP
#define JAREP_VULKANAPI_HPP
#if defined(__linux__) or defined(_WIN32)

#include "IRenderer.hpp"
#include <optional>
#include <set>
#include <vulkan/vulkan.hpp>
#include <fstream>

#if defined (__linux__)
#define VK_USE_PLATFORM_XLIB_KHR

#include <vulkan/vulkan_xlib.h>

#define VK_USE_PLATFORM_WAYLAND_KHR

#include <vulkan/vulkan_wayland.h>

#elif defined(__Win32)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_win32.h>
#endif


namespace Graphics::Vulkan {


	struct QueueFamilyIndices;
	struct SwapChainSupportDetails;

	class VulkanAPI : public IRenderer {
		public:
			VulkanAPI(const std::vector<const char *> &extensionNames);

			~VulkanAPI() override;

			void RegisterPhysicalDevice() override;

			void CreateLogicalDevice() override;

			void CreateSurface(NativeWindowHandleProvider *nativeWindowHandle) override;

			void CreateVertexBuffer() override;

			void CreateShaders() override;

			void CreateCommandQueue() override;

			void CreateGraphicsPipeline() override;

			void RecordCommandBuffer() override;

			void Draw() override;

			void Shutdown() override;

		private:
			VkInstance instance;
			VkPhysicalDevice physicalDevice;
			VkDevice device;
			VkQueue graphicsQueue;
			VkQueue presentQueue;
			VkSurfaceKHR surface;
			VkExtent2D surfaceExtent;
			VkSwapchainKHR swapchain;
			std::vector<VkImage> swapChainImages;
			VkFormat swapchainImageFormat;
			std::vector<VkImageView> swapchainImageViews;
			std::vector<VkShaderModule> shaderModules;
			VkRenderPass renderPass;
			VkPipelineLayout pipelineLayout;
			VkPipeline graphicsPipeline;
			std::vector<VkFramebuffer> swapchainFramebuffers;
			VkCommandPool commandPool;
			VkCommandBuffer commandBuffer;
			VkSemaphore imageAvailableSemaphore;
			VkSemaphore renderFinishedSemaphore;
			VkFence inFlightFence;

			const std::vector<const char *> deviceExtensions = {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			void createVulkanInstance(const std::vector<const char *> &extensionNames);

			bool isPhysicalDeviceSuitable(VkPhysicalDevice device);

			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

			bool checkDeviceExtensionSupport(VkPhysicalDevice device);

			void createLogicalDevice(VkPhysicalDevice physicalDevice);

			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

			void createSwapChain();

			void createImageViews();

			[[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

			static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

			static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

			std::vector<char> readFile(const std::string &filename);

			VkShaderModule createShaderModule(const std::vector<char> &code);

			void createRenderPass();

			void createFramebuffers();

			void createCommandBuffer();

			void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

			void createSyncObjects();
	};
}

#endif
#endif //JAREP_VULKANAPI_HPP
