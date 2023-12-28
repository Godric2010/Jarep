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

        std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& surface) override;

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

        [[nodiscard]] VkSurfaceKHR getSurface() const { return m_surface; }

        [[nodiscard]] VkExtent2D getSurfaceExtent() const { return m_surfaceExtent; }

        SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice);

    private:
        VkSurfaceKHR m_surface;
        VkExtent2D m_surfaceExtent{};
    };


#pragma endregion VulkanSurface }

#pragma region VulkanDevice{

    class VulkanDevice final : public JarDevice
    {
    public:
        VulkanDevice();

        ~VulkanDevice() override;

        void Release() override;

        void CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface>& surface);

        void CreateLogicalDevice();

        void CreateSwapchain(std::shared_ptr<VulkanSurface> surface);

        void CreateImageViews();

        std::shared_ptr<JarBuffer> CreateBuffer(size_t bufferSize, const void* data) override;

        std::shared_ptr<JarShaderModule> CreateShaderModule(std::string fileContent) override;

        std::shared_ptr<JarPipeline> CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule,
                                                    std::shared_ptr<JarShaderModule> fragmentModule,
                                                    std::shared_ptr<JarRenderPass> renderPass) override;

        std::shared_ptr<JarCommandQueue> CreateCommandQueue() override;

        std::shared_ptr<JarRenderPass> CreateRenderPass(std::shared_ptr<JarSurface> surface) override;

        [[nodiscard]] VkDevice getLogicalDevice() const { return m_device; }

        [[nodiscard]] VkSwapchainKHR getSwapchain() const { return m_swapchain; }

        [[nodiscard]] VkQueue getGraphicsQueue() const { return m_graphicsQueue; }

        [[nodiscard]] VkQueue getPresentQueue() const { return m_presentQueue; }

    private:
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        std::optional<uint32_t> m_graphicsFamily;
        std::optional<uint32_t> m_presentFamily;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;
        VkExtent2D m_imageExtent;
        VkFormat m_swapchainImageFormat;
        VkSwapchainKHR m_swapchain;
        std::vector<VkImage> m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;

        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        bool isPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface);

        void findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface);

        bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const;

        [[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                                  const VkExtent2D surfaceExtent) const;

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    };


#pragma endregion VulkanDevice }

#pragma region VulkanCommandBuffer{

    class VulkanCommandBuffer final : JarCommandBuffer
    {
    public:
        VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkSemaphore imageAvailableSemaphore,
                            VkSemaphore renderFinishedSemaphore, VkFence frameInFlightFence);
        ~VulkanCommandBuffer() override;

        void StartRecording(std::shared_ptr<JarDevice> device, std::shared_ptr<JarRenderPass> renderPass,
                            std::shared_ptr<JarFramebuffer> framebuffer) override;
        void EndRecording() override;
        void BindPipeline(std::shared_ptr<JarPipeline> pipeline) override;
        void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) override;
        void Draw() override;
        void Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) override;

        void Release(VkDevice device);

    private:
        VkCommandBuffer m_commandBuffer;
        VkSemaphore m_imageAvailableSemaphore;
        VkSemaphore m_renderFinishedSemaphore;
        VkFence m_frameInFlightFence;
    };

#pragma endregion VulkanCommandBuffer}

#pragma  region VulkanCommandQueue{

    class VulkanCommandQueue final : public JarCommandQueue
    {
    public:
        VulkanCommandQueue();
        ~VulkanCommandQueue() override;

        void CreateVulkanCommandQueue(VkDevice& device, uint32_t graphicsFamilyIndex);

        JarCommandBuffer* getNextCommandBuffer() override;

        void Release(std::shared_ptr<JarDevice> device) override;

    private:
        const int MaxFramesInFlight = 2;

        uint32_t m_graphicsFamily;
        VkCommandPool m_commandPool;
        std::vector<VulkanCommandBuffer*> m_commandBuffers;

        int m_currentBufferIndexInUse;

        void createCommandBuffers(VkDevice& device);
    };

#pragma endregion VulkanCommandQueue}

#pragma region VulkanBuffer{

    class VulkanBuffer final : public JarBuffer
    {
    public:
        VulkanBuffer() = default;
        ~VulkanBuffer() override;

        void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, const size_t bufferSize, const void* data);
        static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                       VkMemoryPropertyFlags properties);

        [[nodiscard]] VkBuffer getBuffer() const { return m_buffer; }

    private:
        VkBuffer m_buffer;
        VkDeviceMemory m_bufferMemory;
    };

#pragma endregion VulkanBuffer}

#pragma region VulkanShaderModule{

    class VulkanShaderModule final : public JarShaderModule
    {
    public:
        VulkanShaderModule() = default;
        ~VulkanShaderModule() override;

        void CreateShaderModule(VkDevice device, std::string shaderContent);
        void Release(std::shared_ptr<JarDevice> jarDevice) override;

        [[nodiscard]] VkShaderModule getShaderModule() const { return m_shaderModule; }

    private:
        VkShaderModule m_shaderModule;
    };

#pragma endregion VulkanShaderModule}

#pragma region VulkanFramebuffer{

    class VulkanFramebuffer final : public JarFramebuffer
    {
    public:
        explicit VulkanFramebuffer(const VkExtent2D framebufferExtent)
        {
            m_framebuffer = nullptr;
            m_framebufferExtent = framebufferExtent;
        }

        ~VulkanFramebuffer() override;

        void CreateFramebuffer(VkDevice device, VkRenderPass renderPass,
                               VkImageView swapchainImageView);

        [[nodiscard]] VkFramebuffer getFramebuffer() const { return m_framebuffer; }
        [[nodiscard]] VkExtent2D getFramebufferExtent() const { return m_framebufferExtent; }

        void Release(std::shared_ptr<JarDevice> device) override;

    private:
        VkFramebuffer m_framebuffer;
        VkExtent2D m_framebufferExtent;
    };

#pragma endregion VulkanFrambuffer}

#pragma region VulkanRenderPass{

    class VulkanRenderPass final : public JarRenderPass
    {
    public:
        VulkanRenderPass() = default;
        ~VulkanRenderPass() override;

        void CreateRenderPass(VkDevice device, VkFormat imageFormat);

        void Release(std::shared_ptr<JarDevice> jarDevice) override;

        [[nodiscard]] VkRenderPass getRenderPass() const { return m_renderPass; }

    private:
        VkRenderPass m_renderPass;
    };

#pragma endregion VulkanRenderPass}

#pragma region VulkanGraphicsPipeline{

    class VulkanGraphicsPipeline final : public JarPipeline
    {
    public:
        VulkanGraphicsPipeline() = default;
        ~VulkanGraphicsPipeline() override;

        void CreateGraphicsPipeline(VkDevice device, VkShaderModule vertexSahderModule,
                                    VkShaderModule fragmentShaderModule, std::shared_ptr<VulkanRenderPass> renderPass);

        void CreateFramebuffers(VkDevice device, VkExtent2D surfaceExtent,
                                std::vector<VkImageView> swapchainImageViews);

        void Release() override;
        std::vector<std::shared_ptr<JarFramebuffer>> GetFramebuffers() override;
        std::shared_ptr<JarRenderPass> GetRenderPass() override;

        [[nodiscard]] VkPipeline getPipeline() const { return m_graphicsPipeline; }

    private:
        VkPipelineLayout m_pipelineLayout;
        VkPipeline m_graphicsPipeline;
        std::vector<std::shared_ptr<VulkanFramebuffer>> m_framebuffers;
        std::shared_ptr<VulkanRenderPass> m_renderPass;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };

#pragma endregion VulkanGraphicsPipeline};


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
