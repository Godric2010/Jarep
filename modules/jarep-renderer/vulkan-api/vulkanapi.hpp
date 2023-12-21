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
#include <vulkan/vulkan_core.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include "sdlsurfaceadapter.hpp"

#if defined (__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan_xlib.h>
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan_wayland.h>

#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif


namespace Graphics::Vulkan
{
#pragma region VulkanBackend{

    class VulkanBackend final : public Backend
    {
    public:
        VulkanBackend(const std::vector<const char*>& extensions);

        ~VulkanBackend() override;

        std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* nativeWindowHandleProvider) override;

        std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface> &surface) override;

    private:
        std::vector<const char*> extensionNames;
        VkInstance instance;

        void createInstance();
    };

#pragma endregion VulkanBackend }

#pragma region VulkanSurface{

    struct SwapChainSupportDetails;

    class VulkanSurface final : public JarSurface
    {
    public:
        VulkanSurface(VkSurfaceKHR surface, VkExtent2D surfaceExtend);

        ~VulkanSurface() override;

        void Update() override;

        //JarRenderPass* CreateRenderPass() override;

        VkSurfaceKHR getSurface() { return m_surface; }

        VkExtent2D getSurfaceExtent() { return m_surfaceExtent; }

        SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice);

    private:
        VkSurfaceKHR m_surface;
        VkExtent2D m_surfaceExtent{};
    };


#pragma endregion VulkanSurface }

    #pragma region VulkanDevice{

        class VulkanDevice final : public JarDevice {

            public:
                VulkanDevice();

                ~VulkanDevice() override;

                void Release() override;

                void CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface> &surface);

                void CreateLogicalDevice();


                std::shared_ptr<JarBuffer> CreateBuffer(size_t bufferSize, const void *data) override;

                std::shared_ptr<JarShaderModule> CreateShaderModule(std::string fileContent) override;

                std::shared_ptr<JarPipeline> CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule, std::shared_ptr<JarShaderModule> fragmentModule) override;

                std::shared_ptr<JarCommandQueue> CreateCommandQueue() override;

            private:
                VkPhysicalDevice m_physicalDevice;
                VkDevice m_device;
                std::optional<uint32_t> m_graphicsFamily;
                std::optional<uint32_t> m_presentFamily;
                VkQueue m_graphicsQueue;
                VkQueue m_presentQueue;

                const std::vector<const char *> deviceExtensions = {
                        VK_KHR_SWAPCHAIN_EXTENSION_NAME
                };

                bool isPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface> &surface);

                void findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface> &surface);

                bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice);
        };


    #pragma endregion VulkanDevice }


    /*
        struct QueueFamilyIndices;


        class VulkanAPI {
            public:
                VulkanAPI(const std::vector<const char *> &extensionNames);

                ~VulkanAPI();

                void RegisterPhysicalDevice();

                void CreateLogicalDevice();

                void CreateSurface(NativeWindowHandleProvider *nativeWindowHandle);

                void CreateVertexBuffer();

                void CreateShaders();

                void CreateCommandQueue();

                void CreateGraphicsPipeline();

                void RecordCommandBuffer();

                void Draw();

                void Shutdown();

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
                std::vector<VkCommandBuffer> commandBuffers;
                std::vector<VkSemaphore> imageAvailableSemaphores;
                std::vector<VkSemaphore> renderFinishedSemaphores;
                std::vector<VkFence> inFlightFences;
                uint32_t currentFrame = 0;
                VkBuffer vertexBuffer;
                VkDeviceMemory vertexBufferMemory;

                const std::vector<const char *> deviceExtensions = {
                        VK_KHR_SWAPCHAIN_EXTENSION_NAME
                };

                const int MAX_FRAMES_IN_FLIGHT = 2;

                void createVulkanInstance(const std::vector<const char *> &extensionNames);

                bool isPhysicalDeviceSuitable(VkPhysicalDevice device);

                QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

                bool checkDeviceExtensionSupport(VkPhysicalDevice device);

                void createLogicalDevice(VkPhysicalDevice physicalDevice);

                SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

                void createSwapChain();

                void cleanupSwapchain();

                void createImageViews();

                [[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

                static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

                static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

                std::vector<char> readFile(const std::string &filename);

                VkShaderModule createShaderModule(const std::vector<char> &code);

                void createRenderPass();

                void createFramebuffers();

                void createCommandBuffers();

                void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

                void createSyncObjects();

                static VkVertexInputBindingDescription getBindingDescription();

                static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

                uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        };
        */
}

#endif
#endif //JAREP_VULKANAPI_HPP
