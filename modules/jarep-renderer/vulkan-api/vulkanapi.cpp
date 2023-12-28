//
// Created by Sebastian Borsch on 24.10.23.
//

#include <IRenderer.hpp>
#include <vulkan/vulkan_core.h>

#if defined(__linux__) or defined(_WIN32)

#include "vulkanapi.hpp"
#include "Vertex.hpp"

namespace Graphics::Vulkan
{
#pragma region VulkanBackend{

    VulkanBackend::VulkanBackend(const std::vector<const char*>& extensions)
    {
        extensionNames = extensions;
        createInstance();
    }

    VulkanBackend::~VulkanBackend() = default;

    std::shared_ptr<JarSurface> VulkanBackend::CreateSurface(NativeWindowHandleProvider* nativeWindowHandleProvider)
    {
        VkExtent2D surfaceExtend = VkExtent2D();
        surfaceExtend.width = nativeWindowHandleProvider->getWindowWidth();
        surfaceExtend.height = nativeWindowHandleProvider->getWindowHeight();

        VkSurfaceKHR surface = VulkanSurfaceAdapter::CreateSurfaceFromNativeHandle(nativeWindowHandleProvider,
            instance);

        return std::make_shared<VulkanSurface>(surface, surfaceExtend);
    }

    std::shared_ptr<JarDevice> VulkanBackend::CreateDevice(std::shared_ptr<JarSurface>& surface)
    {
        auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

        auto device = std::make_shared<VulkanDevice>();
        device->CreatePhysicalDevice(instance, vkSurface);
        device->CreateLogicalDevice();

        vkSurface->FinalizeSurface(device);

        return device;
    }

    void VulkanBackend::createInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "JAREP";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "JAREP";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
        createInfo.ppEnabledExtensionNames = extensionNames.data();
        createInfo.pApplicationInfo = &appInfo;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");
    }

#pragma endregion VulkanBackend }

#pragma region VulkanSurface{

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    VulkanSurface::VulkanSurface(VkSurfaceKHR surface, VkExtent2D surfaceExtend)
    {
        m_surface = surface;
        m_surfaceExtent = surfaceExtend;
    }

    VulkanSurface::~VulkanSurface() = default;

    void VulkanSurface::Update()
    {
    }

    void VulkanSurface::FinalizeSurface(std::shared_ptr<VulkanDevice> device)
    {
        auto swapchainSupport = QuerySwapchainSupport(device->getPhysicalDevice());

        m_swapchain = std::make_unique<VulkanSwapchain>();
        m_swapchain->Initialize(device, m_surfaceExtent, swapchainSupport, m_surface);
    }


    SwapChainSupportDetails VulkanSurface::QuerySwapchainSupport(VkPhysicalDevice physicalDevice) const
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    }

#pragma endregion VulkanSurface }

#pragma region VulkanDevice{

    VulkanDevice::VulkanDevice()
    {
        m_physicalDevice = VK_NULL_HANDLE;
        m_device = VK_NULL_HANDLE;
        m_graphicsFamily = std::nullopt;
        m_presentFamily = std::nullopt;
    }

    VulkanDevice::~VulkanDevice() = default;

    void VulkanDevice::CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface>& surface)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("Failed to find GPU that supports vulkan!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (isPhysicalDeviceSuitable(device, surface))
            {
                m_physicalDevice = device;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("Failed to find a suitable gpu");
    }

    void VulkanDevice::CreateLogicalDevice()
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {m_graphicsFamily.value(), m_presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_FALSE)
        {
            throw std::runtime_error("Failed to create logical device");
        }
    }

    std::shared_ptr<JarBuffer> VulkanDevice::CreateBuffer(size_t bufferSize, const void* data)
    {
        auto buffer = std::make_shared<VulkanBuffer>();
        buffer->CreateBuffer(m_device, m_physicalDevice, bufferSize, data);
        return buffer;
    }

    std::shared_ptr<JarShaderModule> VulkanDevice::CreateShaderModule(std::string fileContent)
    {
        std::shared_ptr<VulkanShaderModule> shaderModule = std::make_shared<VulkanShaderModule>();
        shaderModule->CreateShaderModule(m_device, fileContent);
        return shaderModule;
    }

    std::shared_ptr<JarPipeline> VulkanDevice::CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule,
                                                              std::shared_ptr<JarShaderModule> fragmentModule,
                                                              std::shared_ptr<JarRenderPass> renderPass)
    {
        const auto vkVertexModule = reinterpret_cast<std::shared_ptr<VulkanShaderModule>&>(vertexModule);
        const auto vkFragmentModule = reinterpret_cast<std::shared_ptr<VulkanShaderModule>&>(fragmentModule);
        const auto vkRenderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);

        auto pipeline = std::make_shared<VulkanGraphicsPipeline>();
        pipeline->CreateGraphicsPipeline(m_device, vkVertexModule->getShaderModule(),
                                         vkFragmentModule->getShaderModule(), vkRenderPass);
        return pipeline;
    }

    std::shared_ptr<JarCommandQueue> VulkanDevice::CreateCommandQueue()
    {
        std::shared_ptr<VulkanCommandQueue> commandQueue = std::make_shared<VulkanCommandQueue>();
        commandQueue->CreateVulkanCommandQueue(m_device, m_graphicsFamily.value());
        return commandQueue;
    }

    std::shared_ptr<JarRenderPass> VulkanDevice::CreateRenderPass(std::shared_ptr<JarSurface> surface)
    {
        const auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);
        const auto renderPass = std::make_shared<VulkanRenderPass>();
        renderPass->CreateRenderPass(m_device, vkSurface->getSwapchain()->getSwapchainImageFormat());
        vkSurface->getSwapchain()->CreateFramebuffersFromRenderPass(renderPass);
        return renderPass;
    }

    void VulkanDevice::Release()
    {
        vkDestroyDevice(m_device, nullptr);
    }

    bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice,
                                                std::shared_ptr<VulkanSurface>& surface)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return false;
        std::cout << deviceProperties.deviceName << std::endl;

        findQueueFamilies(vkPhysicalDevice, surface);
        bool extensionSupported = checkDeviceExtensionSupport(vkPhysicalDevice);

        bool swapChainAdequate;
        if (extensionSupported)
        {
            SwapChainSupportDetails swapChainSupportDetails = surface->QuerySwapchainSupport(vkPhysicalDevice);
            swapChainAdequate = !swapChainSupportDetails.formats.empty() &&
                !swapChainSupportDetails.presentModes.empty();
        }

        return m_graphicsFamily.has_value() && m_presentFamily.has_value() && extensionSupported && swapChainAdequate;
    }

    void VulkanDevice::findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface->getSurface(), &presentSupport);

            if (presentSupport)
            {
                m_presentFamily = i;
            }

            if (m_graphicsFamily.has_value() && m_presentFamily.has_value())
                break;

            i++;
        }
    }

    bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        return requiredExtensions.empty();
    }
#pragma endregion VulkanDevice }

#pragma region VulkanSwapchain{

    void VulkanSwapchain::Initialize(const std::shared_ptr<VulkanDevice>& device, const VkExtent2D extent,
                                     const SwapChainSupportDetails& swapchainSupport, const VkSurfaceKHR surface)
    {
        m_device = device;
        m_imageExtent = extent;

        vkGetDeviceQueue(m_device->getLogicalDevice(), m_device->getGraphicsFamilyIndex().value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device->getLogicalDevice(), m_device->getPresentFamilyIndex().value(), 0, &m_presentQueue);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
        const VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
        m_imageExtent = chooseSwapExtent(swapchainSupport.capabilities, m_imageExtent);

        m_swapchainImageFormat = surfaceFormat.format;

        m_swapchainMaxImageCount = swapchainSupport.capabilities.minImageCount + 1;
        if (swapchainSupport.capabilities.maxImageCount > 0 &&
            m_swapchainMaxImageCount > swapchainSupport.capabilities.maxImageCount)
        {
            m_swapchainMaxImageCount = swapchainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = m_swapchainMaxImageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = m_imageExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        const uint32_t queueFamilyIndices[] = {
            device->getGraphicsFamilyIndex().value(), device->getPresentFamilyIndex().value()
        };
        if (device->getGraphicsFamilyIndex() != device->getPresentFamilyIndex())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device->getLogicalDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_device->getLogicalDevice(), m_swapchain, &m_swapchainMaxImageCount, nullptr);
        m_swapchainImages.resize(m_swapchainMaxImageCount);
        vkGetSwapchainImagesKHR(m_device->getLogicalDevice(), m_swapchain, &m_swapchainMaxImageCount,
                                m_swapchainImages.data());

        m_currentImageIndex = 0;
    }

    void VulkanSwapchain::CreateFramebuffersFromRenderPass(const std::shared_ptr<VulkanRenderPass>& renderPass)
    {
        for (const auto& m_swapchainImageView : m_swapchainImageViews)
        {
            auto framebuffer = std::make_shared<VulkanFramebuffer>(m_imageExtent);
            framebuffer->CreateFramebuffer(m_device->getLogicalDevice(), renderPass->getRenderPass(),
                                           m_swapchainImageView);
            m_swapchainFramebuffers.push_back(framebuffer);
        }
    }

    std::shared_ptr<VulkanFramebuffer> VulkanSwapchain::AcquireNewImage(VkSemaphore imageAvailable,
                                                                        VkFence frameInFlight)
    {
        vkWaitForFences(m_device->getLogicalDevice(), 1, &frameInFlight, VK_TRUE, UINT64_MAX);
        vkResetFences(m_device->getLogicalDevice(), 1, &frameInFlight);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_device->getLogicalDevice(), m_swapchain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE,
                              &imageIndex);

        return m_swapchainFramebuffers[imageIndex];
    }

    void VulkanSwapchain::PresentImage(VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence frameInFlight,
                                       VkCommandBuffer* cmdBuffer)
    {
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

        if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, frameInFlight) != VK_SUCCESS)
        {
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

    void VulkanSwapchain::Release()
    {
        for (const auto& framebuffer : m_swapchainFramebuffers)
        {
            framebuffer->Release(m_device);
        }

        for (const auto imageView : m_swapchainImageViews)
        {
            vkDestroyImageView(m_device->getLogicalDevice(), imageView, nullptr);
        }

        for (const auto image : m_swapchainImages)
        {
            vkDestroyImage(m_device->getLogicalDevice(), image, nullptr);
        }

        vkDestroySwapchainKHR(m_device->getLogicalDevice(), m_swapchain, nullptr);
    }


    VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                                 const VkExtent2D surfaceExtent)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent;
            actualExtent.width = std::clamp(surfaceExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(surfaceExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanSwapchain::createImageViews()
    {
        m_swapchainImageViews.resize(m_swapchainImages.size());
        for (size_t i = 0; i < m_swapchainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_swapchainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device->getLogicalDevice(), &createInfo, nullptr, &m_swapchainImageViews[i]) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views");
            }
        }
    }
#pragma endregion VulkanSwapchain}


#pragma region VulkanCommandQueue{


    VulkanCommandQueue::VulkanCommandQueue()
    {
        m_graphicsFamily = -1;
        m_commandPool = nullptr;
        m_commandBuffers = std::vector<VulkanCommandBuffer*>();
        m_currentBufferIndexInUse = 0;
    }

    VulkanCommandQueue::~VulkanCommandQueue() = default;

    void VulkanCommandQueue::CreateVulkanCommandQueue(VkDevice& device, uint32_t graphicsFamilyIndex)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = graphicsFamilyIndex;

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool");
        }

        createCommandBuffers(device);
    }

    void VulkanCommandQueue::createCommandBuffers(VkDevice& device)
    {
        std::vector<VkCommandBuffer> commandBuffers;
        commandBuffers.resize(MaxFramesInFlight);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffer");
        }

        for (int i = 0; i < MaxFramesInFlight; i++)
        {
            VkSemaphore imageAvailableSemaphore;
            VkSemaphore renderFinishedSemaphore;
            VkFence inFlightFence;

            if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create semaphores!");
            }

            m_commandBuffers.push_back(new VulkanCommandBuffer(commandBuffers[i], imageAvailableSemaphore,
                                                               renderFinishedSemaphore, inFlightFence));
        }
    }


    JarCommandBuffer* VulkanCommandQueue::getNextCommandBuffer()
    {
        VulkanCommandBuffer* bufferInFlight = m_commandBuffers[m_currentBufferIndexInUse];

        m_currentBufferIndexInUse = m_currentBufferIndexInUse + 1 % MaxFramesInFlight;

        return reinterpret_cast<JarCommandBuffer*>(bufferInFlight);
    }

    void VulkanCommandQueue::Release(std::shared_ptr<JarDevice> device)
    {
        auto vkDeviceContainer = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);


        for (size_t i = 0; i < MaxFramesInFlight; ++i)
        {
            m_commandBuffers[i]->Release(vkDeviceContainer->getLogicalDevice());
        }
        vkDestroyCommandPool(vkDeviceContainer->getLogicalDevice(), m_commandPool, nullptr);
    }

#pragma endregion VulkanCommandQueue}

#pragma region VulkanCommandBuffer{

    VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkSemaphore imageAvailableSemaphore,
                                             VkSemaphore renderFinishedSemaphore, VkFence frameInFlightFence)
    {
        m_commandBuffer = commandBuffer;
        m_imageAvailableSemaphore = imageAvailableSemaphore;
        m_renderFinishedSemaphore = renderFinishedSemaphore;
        m_frameInFlightFence = frameInFlightFence;
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() = default;


    void VulkanCommandBuffer::StartRecording(std::shared_ptr<JarSurface> surface,
                                             std::shared_ptr<JarRenderPass> renderPass)
    {
        auto vkRenderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);
        auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

        auto vkFramebuffer = vkSurface->getSwapchain()->
                                        AcquireNewImage(m_imageAvailableSemaphore, m_frameInFlightFence);

        vkResetCommandBuffer(m_commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        const VkExtent2D surfaceExtent = vkSurface->getSurfaceExtent();

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkRenderPass->getRenderPass();
        renderPassInfo.framebuffer = vkFramebuffer->getFramebuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = surfaceExtent;
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(surfaceExtent.width);
        viewport.height = static_cast<float>(surfaceExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = surfaceExtent;
        vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

        vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::EndRecording()
    {
        vkCmdEndRenderPass(m_commandBuffer);
        if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void VulkanCommandBuffer::Draw()
    {
        vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
    }

    void VulkanCommandBuffer::Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device)
    {
        const auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);
        vkSurface->getSwapchain()->PresentImage(m_imageAvailableSemaphore, m_renderFinishedSemaphore,
                                                m_frameInFlightFence, &m_commandBuffer);
    }

    void VulkanCommandBuffer::BindPipeline(std::shared_ptr<JarPipeline> pipeline)
    {
        const auto vkPipeline = reinterpret_cast<std::shared_ptr<VulkanGraphicsPipeline>&>(pipeline);
        vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->getPipeline());
    }

    void VulkanCommandBuffer::BindVertexBuffer(std::shared_ptr<JarBuffer> buffer)
    {
        const auto vulkanBuffer = reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(buffer);
        const VkBuffer vertexBuffers[] = {vulkanBuffer->getBuffer()};
        const VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void VulkanCommandBuffer::Release(VkDevice device)
    {
        vkDestroySemaphore(device, m_imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(device, m_renderFinishedSemaphore, nullptr);
        vkDestroyFence(device, m_frameInFlightFence, nullptr);
    }


#pragma endregion VulkanCommandBuffer}

#pragma region VulkanBuffer{

    VulkanBuffer::~VulkanBuffer()
    {
    }

    void VulkanBuffer::CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, const size_t bufferSize,
                                    const void* data)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex buffer!");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, m_buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device, m_buffer, m_bufferMemory, 0);

        void* mappedData;
        vkMapMemory(device, m_bufferMemory, 0, bufferSize, 0, &mappedData);
        memcpy(mappedData, data, bufferSize);
        vkUnmapMemory(device, m_bufferMemory);
    }

    uint32_t VulkanBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                                          VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
        {
            if (typeFilter & 1 << i &&
                (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type!");
    }

#pragma endregion VulkanBuffer}

#pragma region VulkanShaderModule {

    VulkanShaderModule::~VulkanShaderModule()
    {
    }

    void VulkanShaderModule::CreateShaderModule(VkDevice device, std::string shaderContent)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderContent.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderContent.data());

        if (vkCreateShaderModule(device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed o create shader module!");
        }
    }

    void VulkanShaderModule::Release(std::shared_ptr<JarDevice> jarDevice)
    {
        const auto vulkan_device = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(jarDevice);
        vkDestroyShaderModule(vulkan_device->getLogicalDevice(), m_shaderModule, nullptr);
    }


#pragma endregion VulkanShaderModule}

#pragma region VulkanGraphicsPipeline{

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
    }

    void VulkanGraphicsPipeline::CreateGraphicsPipeline(VkDevice device, VkShaderModule vertexShaderModule,
                                                        VkShaderModule fragmentShaderModule,
                                                        std::shared_ptr<VulkanRenderPass> renderPass)
    {
        m_renderPass = renderPass;

        auto shaderModules = std::vector<VkShaderModule>();
        shaderModules.push_back(vertexShaderModule);
        shaderModules.push_back(fragmentShaderModule);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertexShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        auto bindingDescriptions = getBindingDescription();
        auto attributeDescriptions = getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;


        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = renderPass->getRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline");
        }
    }

    std::shared_ptr<JarRenderPass> VulkanGraphicsPipeline::GetRenderPass()
    {
        return m_renderPass;
    }

    void VulkanGraphicsPipeline::Release()
    {
    }

    VkVertexInputBindingDescription VulkanGraphicsPipeline::getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> VulkanGraphicsPipeline::getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

#pragma endregion VulkanGraphicsPipeline}

#pragma region VulkanFramebuffer{

    VulkanFramebuffer::~VulkanFramebuffer()
    {
    }

    void VulkanFramebuffer::Release(std::shared_ptr<JarDevice> device)
    {
        const auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
        vkDestroyFramebuffer(vulkanDevice->getLogicalDevice(), m_framebuffer, nullptr);
    }

    void VulkanFramebuffer::CreateFramebuffer(VkDevice device, VkRenderPass renderPass,
                                              VkImageView swapchainImageView)
    {
        const VkImageView attachments[] = {
            swapchainImageView
        };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_framebufferExtent.width;
        framebufferInfo.height = m_framebufferExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }

#pragma endregion VulkanFramebuffer}

#pragma region VulkanRenderPass{

    VulkanRenderPass::~VulkanRenderPass()
    {
    }

    void VulkanRenderPass::CreateRenderPass(const VkDevice device, const VkFormat imageFormat)
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = imageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanRenderPass::Release(std::shared_ptr<JarDevice> jarDevice)
    {
        const auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(jarDevice);
        vkDestroyRenderPass(vulkanDevice->getLogicalDevice(), m_renderPass, nullptr);
    }

#pragma endregion VulkanRenderPass}

    /*

            struct QueueFamilyIndices {
                std::optional<uint32_t> graphicsFamily;
                std::optional<uint32_t> presentFamily;

                bool isComplete() const {
                    return graphicsFamily.has_value() && presentFamily.has_value();
                }
            };


            VulkanAPI::VulkanAPI(const std::vector<const char *> &extensionNames) {
                createVulkanInstance(extensionNames);
            }

            VulkanAPI::~VulkanAPI() {
            }

            void VulkanAPI::RegisterPhysicalDevice() {
                VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
                uint32_t deviceCount = 0;
                vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
                if (deviceCount == 0)
                    throw std::runtime_error("Failed to find GPU that supports vulkan!");

                std::vector<VkPhysicalDevice> devices(deviceCount);
                vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

                for (const auto &device: devices) {
                    if (isPhysicalDeviceSuitable(device)) {
                        vkPhysicalDevice = device;
                        break;
                    }
                }

                if (vkPhysicalDevice == VK_NULL_HANDLE)
                    throw std::runtime_error("Failed to find a suitable gpu");

                physicalDevice = vkPhysicalDevice;
            }

            void VulkanAPI::CreateLogicalDevice() {
                createLogicalDevice(physicalDevice);
                createSwapChain();
                createImageViews();
            }

            void VulkanAPI::CreateSurface(NativeWindowHandleProvider *nativeWindowHandle) {

                surfaceExtent = {static_cast<uint32_t>(nativeWindowHandle->getWindowWidth()),
                                 static_cast<uint32_t>(nativeWindowHandle->getWindowHeight())};

                switch (nativeWindowHandle->getWindowSystem()) {
                    case Cocoa:
                        throw std::runtime_error("Using a cocoa window with vulkan is not allowed!");
                    case Win32: {
                        VkWin32SurfaceCreateInfoKHR createInfo = {};
                        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                        createInfo.hwnd = reinterpret_cast<HWND>(nativeWindowHandle.getNativeWindowHandle());
                        createInfo.hInstance = GetModuleHandle(nullptr);

                        if(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &m_surface)) {
                            throw std::runtime_error("Failed to create Win32 m_surface");
                        }*
                        return;
                    }
                    case Wayland: {
                        auto waylandWindowHandle = dynamic_cast<WaylandWindowHandleProvider *>(nativeWindowHandle);

                        VkWaylandSurfaceCreateInfoKHR waylandCreateInfo = {};
                        waylandCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
                        waylandCreateInfo.display = waylandWindowHandle->getDisplay();
                        waylandCreateInfo.surface = waylandWindowHandle->getWindowHandle();

                        if (vkCreateWaylandSurfaceKHR(instance, &waylandCreateInfo, nullptr, &surface) != VK_SUCCESS) {
                            throw std::runtime_error("Failed to create Wayland m_surface.");
                        }
                        return;
                    }
                    case X11: {

                        auto xcbWindowHandle = dynamic_cast<XlibWindowHandleProvider *>(nativeWindowHandle);

                        VkXlibSurfaceCreateInfoKHR xlibCreateInfo = {};
                        xlibCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
                        xlibCreateInfo.dpy = xcbWindowHandle->getDisplayConnection();
                        xlibCreateInfo.window = xcbWindowHandle->getXcbWindow();

                        auto returnCode = vkCreateXlibSurfaceKHR(instance, &xlibCreateInfo, nullptr, &surface);
                        if (returnCode != VK_SUCCESS) {
                            throw std::runtime_error("Failed to create XCB Surface!");
                        }
                        return;
                    }
                    default:
                        return;
                }
            }

            void VulkanAPI::Draw() {

                vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
                vkResetFences(device, 1, &inFlightFences[currentFrame]);

                uint32_t imageIndex;
                vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
                                      &imageIndex);

                vkResetCommandBuffer(commandBuffers[currentFrame], 0);
                recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

                VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
                VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = waitSemaphores;
                submitInfo.pWaitDstStageMask = waitStages;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
                VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = signalSemaphores;

                if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to submit draw command buffer");
                }

                VkPresentInfoKHR presentInfo{};
                presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores = signalSemaphores;
                VkSwapchainKHR swapChains[] = {swapchain};
                presentInfo.swapchainCount = 1;
                presentInfo.pSwapchains = swapChains;
                presentInfo.pImageIndices = &imageIndex;
                presentInfo.pResults = nullptr;

                vkQueuePresentKHR(presentQueue, &presentInfo);

                currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
            }

            void VulkanAPI::CreateVertexBuffer() {
                const std::vector<Vertex> vertices = {
                        {{0.0f,  -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                        {{0.5f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
                        {{-0.5f, 0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}}
                };

                VkBufferCreateInfo bufferInfo{};
                bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size = sizeof(vertices[0]) * vertices.size();
                bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create vertex buffer!");
                }

                VkMemoryRequirements memoryRequirements;
                vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

                VkMemoryAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize = memoryRequirements.size;
                allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits,
                                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to allocate vertex buffer memory!");
                }

                vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

                void *data;
                vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
                memcpy(data, vertices.data(), (size_t) bufferInfo.size);
                vkUnmapMemory(device, vertexBufferMemory);
            }

            void VulkanAPI::CreateShaders() {

            }

            void VulkanAPI::CreateCommandQueue() {

                QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

                VkCommandPoolCreateInfo poolInfo{};
                poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

                if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create command pool");
                }

                createCommandBuffers();
                createSyncObjects();
            }

            void VulkanAPI::CreateGraphicsPipeline() {
                auto vertShaderCode = readFile("shaders/triangle_vert.spv");
                auto fragShaderCode = readFile("shaders/triangle_frag.spv");

                VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
                VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

                shaderModules.push_back(vertShaderModule);
                shaderModules.push_back(fragShaderModule);

                VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
                vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                vertShaderStageInfo.module = vertShaderModule;
                vertShaderStageInfo.pName = "main";

                VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
                fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                fragShaderStageInfo.module = fragShaderModule;
                fragShaderStageInfo.pName = "main";

                VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

                std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
                VkPipelineDynamicStateCreateInfo dynamicState{};
                dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
                dynamicState.pDynamicStates = dynamicStates.data();

                auto bindingDescriptions = getBindingDescription();
                auto attributeDescriptions = getAttributeDescriptions();

                VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
                vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInfo.vertexBindingDescriptionCount = 1;
                vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions;
                vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
                vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

                VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
                inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                inputAssembly.primitiveRestartEnable = VK_FALSE;


                VkPipelineViewportStateCreateInfo viewportState{};
                viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportState.viewportCount = 1;
                viewportState.scissorCount = 1;

                VkPipelineRasterizationStateCreateInfo rasterizer{};
                rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizer.depthClampEnable = VK_FALSE;
                rasterizer.rasterizerDiscardEnable = VK_FALSE;
                rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
                rasterizer.lineWidth = 1.0f;
                rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
                rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
                rasterizer.depthBiasEnable = VK_FALSE;
                rasterizer.depthBiasConstantFactor = 0.0f;
                rasterizer.depthBiasClamp = 0.0f;
                rasterizer.depthBiasSlopeFactor = 0.0f;

                VkPipelineMultisampleStateCreateInfo multisampling{};
                multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisampling.sampleShadingEnable = VK_FALSE;
                multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

                VkPipelineColorBlendAttachmentState colorBlendAttachment{};
                colorBlendAttachment.colorWriteMask =
                        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                        VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable = VK_FALSE;

                VkPipelineColorBlendStateCreateInfo colorBlending{};
                colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                colorBlending.logicOpEnable = VK_FALSE;
                colorBlending.logicOp = VK_LOGIC_OP_COPY;
                colorBlending.attachmentCount = 1;
                colorBlending.pAttachments = &colorBlendAttachment;

                VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
                pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutCreateInfo.setLayoutCount = 0;
                pipelineLayoutCreateInfo.pSetLayouts = nullptr;
                pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
                pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

                if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create pipeline layout");
                }

                createRenderPass();

                VkGraphicsPipelineCreateInfo pipelineInfo{};
                pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.stageCount = 2;
                pipelineInfo.pStages = shaderStages;
                pipelineInfo.pVertexInputState = &vertexInputInfo;
                pipelineInfo.pInputAssemblyState = &inputAssembly;
                pipelineInfo.pViewportState = &viewportState;
                pipelineInfo.pRasterizationState = &rasterizer;
                pipelineInfo.pMultisampleState = &multisampling;
                pipelineInfo.pDepthStencilState = nullptr;
                pipelineInfo.pColorBlendState = &colorBlending;
                pipelineInfo.pDynamicState = &dynamicState;
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.renderPass = renderPass;
                pipelineInfo.subpass = 0;
                pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
                pipelineInfo.basePipelineIndex = -1;

                if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
                    VK_SUCCESS) {
                    throw std::runtime_error("failed to create graphics pipeline");
                }

                createFramebuffers();
            }

            void VulkanAPI::RecordCommandBuffer() {

            }

            void VulkanAPI::Shutdown() {

                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
                    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
                    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
                    vkDestroyFence(device, inFlightFences[i], nullptr);
                }
                vkDestroyCommandPool(device, commandPool, nullptr);

                cleanupSwapchain();

                vkDestroyBuffer(device, vertexBuffer, nullptr);
                vkFreeMemory(device, vertexBufferMemory, nullptr);

                vkDestroyPipeline(device, graphicsPipeline, nullptr);
                vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
                vkDestroyRenderPass(device, renderPass, nullptr);

                for (auto shaderModule: shaderModules) {
                    vkDestroyShaderModule(device, shaderModule, nullptr);
                }


                vkDestroySurfaceKHR(instance, surface, nullptr);
                vkDestroyDevice(device, nullptr);
            }

            void VulkanAPI::createVulkanInstance(const std::vector<const char *> &extensionNames) {
                VkApplicationInfo appInfo{};
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pApplicationName = "JAREP";
                appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                appInfo.pEngineName = "JAREP";
                appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
                appInfo.apiVersion = VK_API_VERSION_1_3;

                VkInstanceCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
                createInfo.ppEnabledExtensionNames = extensionNames.data();
                createInfo.pApplicationInfo = &appInfo;

                if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
                    throw std::runtime_error("failed to create instance!");
            }

            bool VulkanAPI::isPhysicalDeviceSuitable(const VkPhysicalDevice device) {
                VkPhysicalDeviceProperties deviceProperties;
                VkPhysicalDeviceFeatures deviceFeatures;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

                if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return false;
                std::cout << deviceProperties.deviceName << std::endl;

                QueueFamilyIndices indices = findQueueFamilies(device);
                bool extensionSupported = checkDeviceExtensionSupport(device);

                bool swapChainAdequate;
                if (extensionSupported) {
                    SwapChainSupportDetails swapChainSupportDetails = querySwapChainSupport(device);
                    swapChainAdequate = !swapChainSupportDetails.formats.empty() &&
                                        !swapChainSupportDetails.presentModes.empty();
                }

                return indices.isComplete() && extensionSupported && swapChainAdequate;
            }

            auto VulkanAPI::findQueueFamilies(const VkPhysicalDevice device) -> QueueFamilyIndices {
                QueueFamilyIndices indices;
                uint32_t queueFamilyCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

                int i = 0;
                for (const auto &queueFamily: queueFamilies) {
                    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        indices.graphicsFamily = i;
                    }

                    VkBool32 presentSupport = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

                    if (presentSupport) {
                        indices.presentFamily = i;
                    }

                    if (indices.isComplete())
                        break;

                    i++;
                }
                return indices;
            }

            void VulkanAPI::createLogicalDevice(const VkPhysicalDevice physicalDevice) {
                QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

                std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
                std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

                float queuePriority = 1.0f;
                for (uint32_t queueFamily: uniqueQueueFamilies) {
                    VkDeviceQueueCreateInfo queueCreateInfo{};
                    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    queueCreateInfo.queueFamilyIndex = queueFamily;
                    queueCreateInfo.queueCount = 1;
                    queueCreateInfo.pQueuePriorities = &queuePriority;
                    queueCreateInfos.push_back(queueCreateInfo);
                }

                VkPhysicalDeviceFeatures deviceFeatures{};

                VkDeviceCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                createInfo.pQueueCreateInfos = queueCreateInfos.data();
                createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
                createInfo.pEnabledFeatures = &deviceFeatures;
                createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
                createInfo.ppEnabledExtensionNames = deviceExtensions.data();
                createInfo.enabledLayerCount = 0;

                if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_FALSE) {
                    throw std::runtime_error("Failed to create logical device");
                }

                vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
                vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
            }

            bool VulkanAPI::checkDeviceExtensionSupport(VkPhysicalDevice device) {
                uint32_t extensionCount;
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

                std::vector<VkExtensionProperties> availableExtensions(extensionCount);
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

                std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
                for (const auto &extension: availableExtensions)
                    requiredExtensions.erase(extension.extensionName);

                return requiredExtensions.empty();
            }

            SwapChainSupportDetails VulkanAPI::querySwapChainSupport(VkPhysicalDevice device) {
                SwapChainSupportDetails details;

                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
                if (formatCount != 0) {
                    details.formats.resize(formatCount);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
                }

                uint32_t presentModeCount = 0;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
                if (presentModeCount != 0) {
                    details.presentModes.resize(presentModeCount);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
                }

                return details;
            }

            void VulkanAPI::createSwapChain() {
                SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
                VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
                VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
                VkExtent2D extent2D = chooseSwapExtent(swapChainSupport.capabilities);

                swapchainImageFormat = surfaceFormat.format;

                uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
                if (swapChainSupport.capabilities.maxImageCount > 0 &&
                    imageCount > swapChainSupport.capabilities.maxImageCount) {
                    imageCount = swapChainSupport.capabilities.maxImageCount;
                }

                VkSwapchainCreateInfoKHR createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                createInfo.surface = surface;
                createInfo.minImageCount = imageCount;
                createInfo.imageFormat = surfaceFormat.format;
                createInfo.imageColorSpace = surfaceFormat.colorSpace;
                createInfo.imageExtent = extent2D;
                createInfo.imageArrayLayers = 1;
                createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
                uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
                if (indices.graphicsFamily != indices.presentFamily) {
                    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                    createInfo.queueFamilyIndexCount = 2;
                    createInfo.pQueueFamilyIndices = queueFamilyIndices;
                } else {
                    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    createInfo.queueFamilyIndexCount = 0;
                    createInfo.pQueueFamilyIndices = nullptr;
                }
                createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
                createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                createInfo.presentMode = presentMode;
                createInfo.clipped = VK_TRUE;
                createInfo.oldSwapchain = VK_NULL_HANDLE;

                if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create swap chain!");
                }

                vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
                swapChainImages.resize(imageCount);
                vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages.data());

            }

            VkSurfaceFormatKHR VulkanAPI::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
                for (const auto &availableFormat: availableFormats) {
                    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                        availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
                        return availableFormat;
                    }
                }
                return availableFormats[0];
            }

            VkPresentModeKHR VulkanAPI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
                for (const auto &availablePresentMode: availablePresentModes) {
                    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                        return availablePresentMode;
                    }
                }
                return VK_PRESENT_MODE_FIFO_KHR;
            }

            VkExtent2D
            VulkanAPI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const {
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

            void VulkanAPI::createImageViews() {
                swapchainImageViews.resize(swapChainImages.size());
                for (size_t i = 0; i < swapChainImages.size(); i++) {
                    VkImageViewCreateInfo createInfo = {};
                    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    createInfo.image = swapChainImages[i];
                    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    createInfo.format = swapchainImageFormat;
                    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    createInfo.subresourceRange.baseMipLevel = 0;
                    createInfo.subresourceRange.levelCount = 1;
                    createInfo.subresourceRange.baseArrayLayer = 0;
                    createInfo.subresourceRange.layerCount = 1;

                    if (vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
                        throw std::runtime_error("failed to create image views");
                    }
                }

            }

            std::vector<char> VulkanAPI::readFile(const std::string &filename) {
                std::ifstream file(filename, std::ios::ate | std::ios::binary);

                if (!file.is_open()) {
                    throw std::runtime_error("failed to open file");
                }

                auto fileSize = (size_t) file.tellg();
                std::vector<char> buffer(fileSize);

                file.seekg(0);
                file.read(buffer.data(), fileSize);

                file.close();
                return buffer;
            }

            VkShaderModule VulkanAPI::createShaderModule(const std::vector<char> &code) {

                VkShaderModuleCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                createInfo.codeSize = code.size();
                createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

                VkShaderModule shaderModule;
                if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                    throw std::runtime_error("failed o create shader module!");
                }
                return shaderModule;

            }

            void VulkanAPI::createRenderPass() {
                VkAttachmentDescription colorAttachment{};
                colorAttachment.format = swapchainImageFormat;
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentReference colorAttachmentRef{};
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass{};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorAttachmentRef;

                VkSubpassDependency dependency{};
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = 0;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

                VkRenderPassCreateInfo renderPassInfo{};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.attachmentCount = 1;
                renderPassInfo.pAttachments = &colorAttachment;
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpass;
                renderPassInfo.dependencyCount = 1;
                renderPassInfo.pDependencies = &dependency;

                if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create render pass!");
                }

            }

            void VulkanAPI::createFramebuffers() {
                swapchainFramebuffers.resize(swapchainImageViews.size());
                for (size_t i = 0; i < swapchainImageViews.size(); i++) {
                    VkImageView attachments[] = {
                            swapchainImageViews[i]
                    };

                    VkFramebufferCreateInfo framebufferInfo{};
                    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                    framebufferInfo.renderPass = renderPass;
                    framebufferInfo.attachmentCount = 1;
                    framebufferInfo.pAttachments = attachments;
                    framebufferInfo.width = surfaceExtent.width;
                    framebufferInfo.height = surfaceExtent.height;
                    framebufferInfo.layers = 1;

                    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
                        throw std::runtime_error("Failed to create framebuffer");
                    }
                }

            }

            void VulkanAPI::createCommandBuffers() {

                commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

                VkCommandBufferAllocateInfo allocateInfo{};
                allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocateInfo.commandPool = commandPool;
                allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocateInfo.commandBufferCount = (uint32_t) commandBuffers.size();

                if (vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
                    throw std::runtime_error("failed to allocate command buffer");
                }
            }

            void VulkanAPI::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = 0;
                beginInfo.pInheritanceInfo = nullptr;

                if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                    throw std::runtime_error("failed to begin recording command buffer");
                }

                VkRenderPassBeginInfo renderPassInfo{};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassInfo.renderPass = renderPass;
                renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = surfaceExtent;
                VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0}}};
                renderPassInfo.clearValueCount = 1;
                renderPassInfo.pClearValues = &clearColor;

                vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

                VkBuffer vertexBuffers[] = {vertexBuffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float) surfaceExtent.width;
                viewport.height = (float) surfaceExtent.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

                VkRect2D scissor{};
                scissor.offset = {0, 0};
                scissor.extent = surfaceExtent;
                vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

                vkCmdDraw(commandBuffer, 3, 1, 0, 0);

                vkCmdEndRenderPass(commandBuffer);
                if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }

            void VulkanAPI::createSyncObjects() {

                imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
                renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
                inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

                VkSemaphoreCreateInfo semaphoreCreateInfo{};
                semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                VkFenceCreateInfo fenceInfo{};
                fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                    if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                        throw std::runtime_error("failed to create semaphores!");
                    }
                }
            }

            void VulkanAPI::cleanupSwapchain() {

                for (size_t i = 0; i < swapchainFramebuffers.size(); ++i) {
                    vkDestroyFramebuffer(device, swapchainFramebuffers[i], nullptr);
                }
                for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
                    vkDestroyImageView(device, swapchainImageViews[i], nullptr);
                }
                vkDestroySwapchainKHR(device, swapchain, nullptr);

            }

            VkVertexInputBindingDescription VulkanAPI::getBindingDescription() {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return bindingDescription;
            }

            std::array<VkVertexInputAttributeDescription, 2> VulkanAPI::getAttributeDescriptions() {
                std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, position);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, color);

                return attributeDescriptions;
            }

            uint32_t VulkanAPI::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
                VkPhysicalDeviceMemoryProperties memoryProperties;
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
                for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
                    if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                        return i;
                    }
                }
                throw std::runtime_error("Failed to find suitable memory type!");
            }*/
}
#endif
