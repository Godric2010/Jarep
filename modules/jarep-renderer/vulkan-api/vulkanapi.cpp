//
// Created by Sebastian Borsch on 24.10.23.
//

#if defined(__linux__) or defined(_WIN32)

#include "vulkanapi.hpp"
#include "Vertex.hpp"

namespace Graphics::Vulkan {
#pragma region VulkanBackend{

	VulkanBackend::VulkanBackend(const std::vector<const char*>& extensions) {
		extensionNames = extensions;

		bool enable_debug_mode = false;
#ifdef ENABLE_VALIDATION_LAYERS
		enable_debug_mode = true;
#endif

		if (enable_debug_mode) {
			enableValidationLayers();
			createInstance();
			createDebugCallbackSender();
		} else {
			createInstance();
		}

		m_bufferCount = 0;
	}

	VulkanBackend::~VulkanBackend() = default;

	BackendType VulkanBackend::GetType() {
		return BackendType::Vulkan;
	}

	std::shared_ptr<JarSurface> VulkanBackend::CreateSurface(NativeWindowHandleProvider* nativeWindowHandleProvider) {
		VkExtent2D surfaceExtend = VkExtent2D();
		surfaceExtend.width = nativeWindowHandleProvider->getWindowWidth();
		surfaceExtend.height = nativeWindowHandleProvider->getWindowHeight();

		VkSurfaceKHR surface = VulkanSurfaceAdapter::CreateSurfaceFromNativeHandle(nativeWindowHandleProvider,
		                                                                           instance);

		return std::make_shared<VulkanSurface>(surface, surfaceExtend);
	}

	std::shared_ptr<JarDevice> VulkanBackend::CreateDevice(std::shared_ptr<JarSurface>& surface) {
		auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

		m_device = std::make_shared<VulkanDevice>();
		m_device->CreatePhysicalDevice(instance, vkSurface);
		m_device->CreateLogicalDevice();

		vkSurface->FinalizeSurface(m_device);
		return m_device;
	}

	JarShaderModuleBuilder* VulkanBackend::InitShaderModuleBuilder() {
		return new VulkanShaderModuleBuilder();
	}

	JarRenderPassBuilder* VulkanBackend::InitRenderPassBuilder() {
		return new VulkanRenderPassBuilder();
	}

	JarCommandQueueBuilder* VulkanBackend::InitCommandQueueBuilder() {
		return new VulkanCommandQueueBuilder();
	};

	JarBufferBuilder* VulkanBackend::InitBufferBuilder() {
		return new VulkanBufferBuilder(static_cast<std::shared_ptr<VulkanBackend>>(this));
	}

	JarImageBuilder* VulkanBackend::InitImageBuilder() {
		return new VulkanImageBuilder(static_cast<std::shared_ptr<VulkanBackend>>(this));
	}

	JarPipelineBuilder* VulkanBackend::InitPipelineBuilder() {
		return new VulkanGraphicsPipelineBuilder();
	}

	JarDescriptorBuilder* VulkanBackend::InitDescriptorBuilder() {
		return new VulkanDescriptorBuilder();
	}

	std::shared_ptr<VulkanCommandQueue> VulkanBackend::getStagingCommandQueue() {
		if (m_stagingCommandQueue == nullptr) {
			auto jarQueue = VulkanCommandQueueBuilder().SetCommandBufferAmount(2)->Build(m_device);
			m_stagingCommandQueue = reinterpret_cast<std::shared_ptr<VulkanCommandQueue>&>(jarQueue);
		}
		return m_stagingCommandQueue;
	}

	void VulkanBackend::onRegisterNewBuffer() {
		m_bufferCount++;
	}

	void VulkanBackend::onDestroyBuffer() {
		m_bufferCount--;
		if (m_bufferCount <= 0)
			m_stagingCommandQueue->Release();
	}


	void VulkanBackend::createInstance() {
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
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.pApplicationInfo = &appInfo;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("failed to create instance!");


	}

	void VulkanBackend::enableValidationLayers() {
		extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		validationLayers = {
				"VK_LAYER_KHRONOS_validation"
		};

		for (const char* layer_name: validationLayers) {
			bool layer_found = false;

			for (const auto& layer_properties: available_layers) {
				if (strcmp(layer_name, layer_properties.layerName) == 0) {
					layer_found = true;
					break;
				}
			}

			if (!layer_found)
				throw std::runtime_error("Layer not found");
		}
	}

	void VulkanBackend::createDebugCallbackSender() {
		PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger_ext = nullptr;
		create_debug_utils_messenger_ext = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
				instance, "vkCreateDebugUtilsMessengerEXT"));

		if (!create_debug_utils_messenger_ext)
			throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT");


		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		if (create_debug_utils_messenger_ext(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

#pragma endregion VulkanBackend }

#pragma region VulkanSurface{


	VulkanSurface::VulkanSurface(VkSurfaceKHR surface, VkExtent2D surfaceExtend) {
		m_surface = surface;
		m_surfaceExtent = surfaceExtend;
	}

	VulkanSurface::~VulkanSurface() = default;

	void VulkanSurface::RecreateSurface(uint32_t width, uint32_t height) {

		vkDeviceWaitIdle(m_device->getLogicalDevice());

		m_surfaceExtent.width = width;
		m_surfaceExtent.height = height;

		auto swapchainSupport = QuerySwapchainSupport(m_device->getPhysicalDevice());

		m_swapchain->RecreateSwapchain(width, height, swapchainSupport);
	}

	void VulkanSurface::ReleaseSwapchain() {
		m_swapchain->Release();
	}

	uint32_t VulkanSurface::GetSwapchainImageAmount() {
		uint32_t maxSwapchainImages = m_swapchain->getMaxSwapchainImageCount();
		return maxSwapchainImages;
	}

	JarExtent VulkanSurface::GetSurfaceExtent() {
		JarExtent extent{};
		extent.Width = m_surfaceExtent.width;
		extent.Height = m_surfaceExtent.height;
		return extent;
	}

	void VulkanSurface::FinalizeSurface(std::shared_ptr<VulkanDevice> device) {
		m_device = device;

		auto swapchainSupport = QuerySwapchainSupport(m_device->getPhysicalDevice());

		m_swapchain = std::make_unique<VulkanSwapchain>(m_device, m_surface);
		m_swapchain->Initialize(m_surfaceExtent, swapchainSupport);
	}


	SwapChainSupportDetails VulkanSurface::QuerySwapchainSupport(VkPhysicalDevice physicalDevice) const {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &details.capabilities);


		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount,
			                                          details.presentModes.data());
		}

		return details;
	}

#pragma endregion VulkanSurface }

#pragma region VulkanDevice{

	VulkanDevice::VulkanDevice() {
		m_physicalDevice = VK_NULL_HANDLE;
		m_device = VK_NULL_HANDLE;
		m_graphicsFamily = std::nullopt;
		m_presentFamily = std::nullopt;
	}

	VulkanDevice::~VulkanDevice() = default;

	void VulkanDevice::CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface>& surface) {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPU that supports vulkan!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device: devices) {
			if (isPhysicalDeviceSuitable(device, surface)) {
				m_physicalDevice = device;
				break;
			}
		}

		if (m_physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable gpu");
	}

	void VulkanDevice::CreateLogicalDevice() {
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {m_graphicsFamily.value(), m_presentFamily.value()};

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
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_FALSE) {
			throw std::runtime_error("Failed to create logical device");
		}
	}

	void VulkanDevice::Release() {
		vkDestroyDevice(m_device, nullptr);
	}

	uint32_t VulkanDevice::GetMaxUsableSampleCount() {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
		                            physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return 64; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return 32; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return 16; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return 8; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return 4; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return 2; }

		return 1;
	}

	bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice,
	                                            std::shared_ptr<VulkanSurface>& surface) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return false;
		std::cout << deviceProperties.deviceName << std::endl;

		findQueueFamilies(vkPhysicalDevice, surface);
		bool extensionSupported = checkDeviceExtensionSupport(vkPhysicalDevice);

		bool swapChainAdequate;
		if (extensionSupported) {
			SwapChainSupportDetails swapChainSupportDetails = surface->QuerySwapchainSupport(vkPhysicalDevice);
			swapChainAdequate = !swapChainSupportDetails.formats.empty() &&
			                    !swapChainSupportDetails.presentModes.empty();
		}

		return m_graphicsFamily.has_value() && m_presentFamily.has_value() && extensionSupported && swapChainAdequate &&
		       deviceFeatures.samplerAnisotropy;
	}

	void VulkanDevice::findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily: queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				m_graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface->getSurface(), &presentSupport);

			if (presentSupport) {
				m_presentFamily = i;
			}

			if (m_graphicsFamily.has_value() && m_presentFamily.has_value())
				break;

			i++;
		}
	}

	bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension: availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

#pragma endregion VulkanDevice }

#pragma region VulkanSwapchain{

	void VulkanSwapchain::Initialize(VkExtent2D extent, SwapChainSupportDetails swapchainSupport) {
		m_swapchainSupport = std::move(swapchainSupport);
		m_imageExtent = extent;

		vkGetDeviceQueue(m_device->getLogicalDevice(), m_device->getGraphicsFamilyIndex().value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device->getLogicalDevice(), m_device->getPresentFamilyIndex().value(), 0, &m_presentQueue);

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
				m_device->getGraphicsFamilyIndex().value(), m_device->getPresentFamilyIndex().value()
		};
		if (m_device->getGraphicsFamilyIndex() != m_device->getPresentFamilyIndex()) {
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

		if (vkCreateSwapchainKHR(m_device->getLogicalDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_device->getLogicalDevice(), m_swapchain, &m_swapchainMaxImageCount, nullptr);
		m_swapchainImages.resize(m_swapchainMaxImageCount);
		vkGetSwapchainImagesKHR(m_device->getLogicalDevice(), m_swapchain, &m_swapchainMaxImageCount,
		                        m_swapchainImages.data());

		createImageViews();
		m_currentImageIndex = 0;
	}

	std::optional<uint32_t> VulkanSwapchain::AcquireNewImage(VkSemaphore imageAvailable,
	                                                         VkFence frameInFlight) {
		vkWaitForFences(m_device->getLogicalDevice(), 1, &frameInFlight, VK_TRUE, UINT64_MAX);


		auto result = vkAcquireNextImageKHR(m_device->getLogicalDevice(), m_swapchain, UINT64_MAX, imageAvailable,
		                                    VK_NULL_HANDLE,
		                                    &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			std::cout << " swapchain out of date" << std::endl;
			return std::nullopt;
		}
		vkResetFences(m_device->getLogicalDevice(), 1, &frameInFlight);
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

		if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, frameInFlight) != VK_SUCCESS) {
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
			vkDestroyImageView(m_device->getLogicalDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_device->getLogicalDevice(), m_swapchain, nullptr);
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

			VulkanImageBuilder::createImageView(m_device, m_swapchainImages[i], m_swapchainImageFormat,
			                                    VK_IMAGE_ASPECT_COLOR_BIT,
			                                    &m_swapchainImageViews[i], 1);
		}
	}

	VkFormat VulkanSwapchain::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
	                                              VkFormatFeatureFlags features) {
		for (VkFormat format: candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_device->getPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat VulkanSwapchain::findDepthFormat() {
		return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		                           VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool VulkanSwapchain::hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
	}


#pragma endregion VulkanSwapchain }

#pragma region VulkanCommandQueue{

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


	VulkanCommandQueue::~VulkanCommandQueue() = default;


	JarCommandBuffer* VulkanCommandQueue::getNextCommandBuffer() {
		VulkanCommandBuffer* bufferInFlight = m_commandBuffers[m_currentBufferIndexInUse];

		m_currentBufferIndexInUse = (m_currentBufferIndexInUse + 1) % m_commandBuffers.size();

		return reinterpret_cast<JarCommandBuffer*>(bufferInFlight);
	}

	void VulkanCommandQueue::Release() {

		for (auto& m_commandBuffer: m_commandBuffers) {
			m_commandBuffer->Release(m_device->getLogicalDevice());
		}
		vkDestroyCommandPool(m_device->getLogicalDevice(), m_commandPool, nullptr);
	}

#pragma endregion VulkanCommandQueue }

#pragma region VulkanCommandBuffer{

	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkSemaphore imageAvailableSemaphore,
	                                         VkSemaphore renderFinishedSemaphore, VkFence frameInFlightFence) {
		m_commandBuffer = commandBuffer;
		m_imageAvailableSemaphore = imageAvailableSemaphore;
		m_renderFinishedSemaphore = renderFinishedSemaphore;
		m_frameInFlightFence = frameInFlightFence;
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

	bool VulkanCommandBuffer::StartRecording(std::shared_ptr<JarSurface> surface,
	                                         std::shared_ptr<JarRenderPass> renderPass) {
		auto vkRenderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);
		auto vkSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

		auto currentFrameIndex = vkSurface->getSwapchain()->
				AcquireNewImage(m_imageAvailableSemaphore, m_frameInFlightFence);

		if (!currentFrameIndex.has_value()) {
			return false;
		}
		auto vkFramebuffer = vkRenderPass->AcquireNextFramebuffer(currentFrameIndex.value());

		vkResetCommandBuffer(m_commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		const VkExtent2D surfaceExtent = vkSurface->getSurfaceExtent();

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vkRenderPass->getRenderPass();
		renderPassInfo.framebuffer = vkFramebuffer->getFramebuffer();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = surfaceExtent;
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


	VkCommandBuffer VulkanCommandBuffer::StartSingleTimeRecording(std::shared_ptr<VulkanDevice>& device,
	                                                              std::shared_ptr<VulkanCommandQueue>& commandQueue) {

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandQueue->getCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device->getLogicalDevice(), &allocInfo, &commandBuffer);

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
		vkGetDeviceQueue(vulkanDevice->getLogicalDevice(), vulkanDevice->getGraphicsFamilyIndex().value(), 0,
		                 &graphicsQueue);
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(vulkanDevice->getLogicalDevice(), commandQueue->getCommandPool(), 1, &commandBuffer);

	}


#pragma endregion VulkanCommandBuffer }

#pragma region VulkanBuffer{

	static std::unordered_map<BufferUsage, VkBufferUsageFlags> bufferUsageMap{
			{BufferUsage::VertexBuffer,  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT},
			{BufferUsage::IndexBuffer,   VK_BUFFER_USAGE_INDEX_BUFFER_BIT},
			{BufferUsage::UniformBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},
			{BufferUsage::StoreBuffer,   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},
			{BufferUsage::TransferSrc,   VK_BUFFER_USAGE_TRANSFER_SRC_BIT},
			{BufferUsage::TransferDest,  VK_BUFFER_USAGE_TRANSFER_DST_BIT},
	};

	VulkanBufferBuilder::~VulkanBufferBuilder() = default;

	VulkanBufferBuilder* VulkanBufferBuilder::SetUsageFlags(Graphics::BufferUsage usageFlags) {
		m_bufferUsageFlags = std::make_optional<VkBufferUsageFlags>(bufferUsageMap[usageFlags]);
		return this;
	}

	VulkanBufferBuilder* VulkanBufferBuilder::SetMemoryProperties(Graphics::MemoryProperties memProps) {

		VkMemoryPropertyFlags vkFlags = 0;

		if (memProps.flags & MemoryProperties::DeviceLocal) vkFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if (memProps.flags & MemoryProperties::HostVisible) vkFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		if (memProps.flags & MemoryProperties::HostCoherent) vkFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		if (memProps.flags & MemoryProperties::HostCached) vkFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		if (memProps.flags & MemoryProperties::LazilyAllocation) vkFlags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

		m_memoryPropertiesFlags = std::make_optional(vkFlags);
		return this;
	}

	VulkanBufferBuilder* VulkanBufferBuilder::SetBufferData(const void* data, size_t bufferSize) {
		m_bufferSize = std::make_optional<size_t>(bufferSize);
		m_data = std::make_optional(data);
		return this;
	}

	std::shared_ptr<JarBuffer> VulkanBufferBuilder::Build(std::shared_ptr<JarDevice> device) {

		if (!m_bufferSize.has_value() || !m_data.has_value() || !m_memoryPropertiesFlags.has_value() ||
		    !m_bufferUsageFlags.has_value())
			throw std::runtime_error("Buffer not correctly initialized! All fields must be set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		if (m_bufferUsageFlags.value() == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
			return BuildUniformBuffer(vulkanDevice);
		}

		m_backend->onRegisterNewBuffer();
		auto size = m_bufferSize.value();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vulkanDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
		             stagingBufferMemory);

		void* stagingData;
		void* data = const_cast<void*>(m_data.value());
		vkMapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, 0, size, 0, &stagingData);
		memcpy(stagingData, data, size);
		vkUnmapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory);


		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		createBuffer(vulkanDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | m_bufferUsageFlags.value(),
		             m_memoryPropertiesFlags.value(), buffer, bufferMemory);

		copyBuffer(vulkanDevice, stagingBuffer, buffer, size);

		vkDestroyBuffer(vulkanDevice->getLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, nullptr);

		auto releasedCallback = [this]() { m_backend->onDestroyBuffer(); };
		return std::make_shared<VulkanBuffer>(vulkanDevice, nextBufferId++, buffer, m_bufferSize.value(), bufferMemory,
		                                      data,
		                                      releasedCallback);
	}

	void VulkanBufferBuilder::createBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkDeviceSize size,
	                                       VkBufferUsageFlags usage,
	                                       VkMemoryPropertyFlags properties, VkBuffer& buffer,
	                                       VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vulkanDevice->getLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create vertex buffer!");
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(vulkanDevice->getLogicalDevice(), buffer, &memoryRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(vulkanDevice->getPhysicalDevice(), memoryRequirements.memoryTypeBits,
		                                           properties);


		if (vkAllocateMemory(vulkanDevice->getLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(vulkanDevice->getLogicalDevice(), buffer, bufferMemory, 0);
	}

	void
	VulkanBufferBuilder::copyBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer srcBuffer, VkBuffer dstBuffer,
	                                VkDeviceSize size) {

		auto vulkanCommandPool = m_backend->getStagingCommandQueue();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(vulkanDevice, vulkanCommandPool);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		VulkanCommandBuffer::EndSingleTimeRecording(vulkanDevice, commandBuffer, vulkanCommandPool);

	}

	uint32_t VulkanBufferBuilder::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
	                                             VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			if (typeFilter & 1 << i &&
			    (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type!");
	}

	std::shared_ptr<JarBuffer> VulkanBufferBuilder::BuildUniformBuffer(std::shared_ptr<VulkanDevice> vulkanDevice) {

		VkBuffer buffer;
		VkDeviceMemory bufferMemory;


		createBuffer(vulkanDevice, m_bufferSize.value(), m_bufferUsageFlags.value(), m_memoryPropertiesFlags.value(),
		             buffer, bufferMemory);
		void* data;
		vkMapMemory(vulkanDevice->getLogicalDevice(), bufferMemory, 0, m_bufferSize.value(), 0, &data);

		auto releasedCallback = []() {};
		return std::make_shared<VulkanBuffer>(vulkanDevice, nextBufferId++, buffer, m_bufferSize.value(), bufferMemory,
		                                      data,
		                                      releasedCallback);
	}

	VulkanBuffer::~VulkanBuffer() = default;

	void VulkanBuffer::Release() {
		vkFreeMemory(m_device->getLogicalDevice(), m_bufferMemory, nullptr);
		vkDestroyBuffer(m_device->getLogicalDevice(), m_buffer, nullptr);

		m_bufferReleasedCallback();
	}

	void VulkanBuffer::Update(const void* data, size_t bufferSize) {
		memcpy(m_data, data, bufferSize);
		m_bufferSize = bufferSize;
	}


#pragma endregion VulkanBuffer }

#pragma region VulkanDescriptorSet{

	VulkanDescriptorBuilder::~VulkanDescriptorBuilder() = default;

	VulkanDescriptorBuilder* VulkanDescriptorBuilder::SetBinding(uint32_t binding) {
		m_binding = std::make_optional<uint32_t>(binding);
		return this;
	}

	VulkanDescriptorBuilder* VulkanDescriptorBuilder::SetStageFlags(StageFlags stageFlags) {
		auto vkStageFlags = convertToVkShaderStageFlagBits(stageFlags);
		m_stageFlags = std::make_optional<VkShaderStageFlagBits>(vkStageFlags);
		return this;
	}

	std::shared_ptr<JarDescriptor>
	VulkanDescriptorBuilder::BuildUniformBufferDescriptor(std::shared_ptr<JarDevice> device,
	                                                      std::vector<std::shared_ptr<JarBuffer>> uniformBuffers) {
		if (!m_stageFlags.has_value())
			throw std::runtime_error("StageFlags not set!");

		if (!m_binding.has_value())
			throw std::runtime_error("Binding not set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		std::vector<std::shared_ptr<VulkanBuffer>> vulkanUniformBuffers;
		for (auto& buffer: uniformBuffers) {
			vulkanUniformBuffers.push_back(reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(buffer));
		}
		auto descriptorAmount = vulkanUniformBuffers.size();

		auto descriptorLayout = BuildDescriptorLayout(vulkanDevice, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		auto descriptorPool = BuildDescriptorPool(vulkanDevice, descriptorAmount, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		auto descriptorSets = AllocateDescriptorSets(vulkanDevice, descriptorPool, descriptorLayout, descriptorAmount);

		for (int i = 0; i < descriptorSets.size(); ++i) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = vulkanUniformBuffers[i]->getBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = vulkanUniformBuffers[i]->getBufferSize();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = m_binding.value();
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
			vkUpdateDescriptorSets(vulkanDevice->getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		auto vulkanDescriptor = std::make_shared<VulkanDescriptor>(vulkanDevice, descriptorPool, descriptorLayout,
		                                                           descriptorSets);
		return vulkanDescriptor;
	}

	std::shared_ptr<JarDescriptor> VulkanDescriptorBuilder::BuildImageBufferDescriptor(
			std::shared_ptr<JarDevice> device, std::shared_ptr<JarImage> image) {
		if (!m_stageFlags.has_value())
			throw std::runtime_error("StageFlags not set!");

		if (!m_binding.has_value())
			throw std::runtime_error("Binding not set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		auto vulkanImage = reinterpret_cast<std::shared_ptr<VulkanImage>&>(image);
		uint32_t descriptorAmount = 1;

		auto descriptorLayout = BuildDescriptorLayout(vulkanDevice, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		auto descriptorPool = BuildDescriptorPool(vulkanDevice, descriptorAmount,
		                                          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		auto descriptorSets = AllocateDescriptorSets(vulkanDevice, descriptorPool, descriptorLayout, descriptorAmount);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = vulkanImage->getImageView();
		imageInfo.sampler = vulkanImage->getSampler();

		VkWriteDescriptorSet descriptorWriteSampler{};
		descriptorWriteSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriteSampler.dstSet = descriptorSets[0];
		descriptorWriteSampler.dstBinding = m_binding.value();
		descriptorWriteSampler.dstArrayElement = 0;
		descriptorWriteSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWriteSampler.descriptorCount = 1;
		descriptorWriteSampler.pBufferInfo = nullptr;
		descriptorWriteSampler.pImageInfo = &imageInfo;
		descriptorWriteSampler.pTexelBufferView = nullptr;


		vkUpdateDescriptorSets(vulkanDevice->getLogicalDevice(), 1, &descriptorWriteSampler, 0, nullptr);

		auto vulkanDescriptor = std::make_shared<VulkanDescriptor>(vulkanDevice, descriptorPool, descriptorLayout,
		                                                           descriptorSets);
		return vulkanDescriptor;
	}


	std::shared_ptr<VulkanDescriptorLayout>
	VulkanDescriptorBuilder::BuildDescriptorLayout(std::shared_ptr<VulkanDevice> vulkanDevice,
	                                               VkDescriptorType descriptorType) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = m_binding.value();
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = m_stageFlags.value();
		layoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = 1;
		layoutCreateInfo.pBindings = &layoutBinding;

		VkDescriptorSetLayout descriptorSetLayout;
		if (vkCreateDescriptorSetLayout(vulkanDevice->getLogicalDevice(), &layoutCreateInfo, nullptr,
		                                &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderStepDescriptor set layout!");
		}
		auto vulkanDescriptorLayout = std::make_shared<VulkanDescriptorLayout>(vulkanDevice, descriptorSetLayout,
		                                                                       m_binding.value());
		return vulkanDescriptorLayout;
	}

	VkDescriptorPool VulkanDescriptorBuilder::BuildDescriptorPool(std::shared_ptr<VulkanDevice> vulkanDevice,
	                                                              uint32_t descriptorSetCount,
	                                                              VkDescriptorType descriptorType) {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = descriptorType;
		poolSize.descriptorCount = descriptorSetCount;

		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = 1;
		poolCreateInfo.pPoolSizes = &poolSize;
		poolCreateInfo.maxSets = descriptorSetCount;

		VkDescriptorPool descriptorPool;
		if (vkCreateDescriptorPool(vulkanDevice->getLogicalDevice(), &poolCreateInfo, nullptr, &descriptorPool) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderStepDescriptor pool!");
		}
		return descriptorPool;
	}

	std::vector<VkDescriptorSet>
	VulkanDescriptorBuilder::AllocateDescriptorSets(std::shared_ptr<VulkanDevice> vulkanDevice,
	                                                VkDescriptorPool descriptorPool,
	                                                std::shared_ptr<VulkanDescriptorLayout> descriptorLayout,
	                                                uint32_t descriptorSetCount) {
		std::vector<VkDescriptorSetLayout> layouts(descriptorSetCount, descriptorLayout->getDescriptorSetLayout());
		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = descriptorPool;
		allocateInfo.descriptorSetCount = descriptorSetCount;
		allocateInfo.pSetLayouts = layouts.data();

		std::vector<VkDescriptorSet> descriptorSets(descriptorSetCount);
		if (vkAllocateDescriptorSets(vulkanDevice->getLogicalDevice(), &allocateInfo, descriptorSets.data()) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderStepDescriptor sets!");
		}
		return descriptorSets;
	}

	VkShaderStageFlagBits
	VulkanDescriptorBuilder::convertToVkShaderStageFlagBits(Graphics::StageFlags stageFlags) {
		int flagBits = 0;
		auto maskValue = static_cast<std::underlying_type<StageFlags>::type>(stageFlags);
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::VertexShader))
			flagBits |= VK_SHADER_STAGE_VERTEX_BIT;
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::FragmentShader))
			flagBits |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::GeometryShader))
			flagBits |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::ComputeShader))
			flagBits |= VK_SHADER_STAGE_COMPUTE_BIT;
		return static_cast<VkShaderStageFlagBits>(flagBits);

	}

	VulkanDescriptorLayout::~VulkanDescriptorLayout() = default;

	void VulkanDescriptorLayout::Release() {
		vkDestroyDescriptorSetLayout(m_device->getLogicalDevice(), m_descriptorSetLayout, nullptr);
	}

	VulkanDescriptor::~VulkanDescriptor() = default;

	void VulkanDescriptor::Release() {
		vkDestroyDescriptorPool(m_device->getLogicalDevice(), m_descriptorPool, nullptr);
		m_descriptorSetLayout->Release();
	}

	std::shared_ptr<JarDescriptorLayout> VulkanDescriptor::GetDescriptorLayout() {
		return m_descriptorSetLayout;
	}

	VkDescriptorSet VulkanDescriptor::GetNextDescriptorSet() {

		auto descriptorSet = m_descriptorSets[m_descriptorSetIndex];
		m_descriptorSetIndex = (m_descriptorSetIndex + 1) % m_descriptorSets.size();
		return descriptorSet;
	}


#pragma endregion VulkanDescriptorSet }

#pragma region VulkanShaderModule {

	VulkanShaderModuleBuilder::~VulkanShaderModuleBuilder() = default;

	VulkanShaderModuleBuilder* VulkanShaderModuleBuilder::SetShader(std::string shaderCode) {
		m_shaderCodeStr = std::make_optional(shaderCode);
		return this;
	}

	VulkanShaderModuleBuilder* VulkanShaderModuleBuilder::SetShaderType(Graphics::ShaderType shaderType) {
		m_shaderType = std::make_optional(shaderType);
		return this;
	}

	std::shared_ptr<JarShaderModule> VulkanShaderModuleBuilder::Build(std::shared_ptr<JarDevice> device) {

		if (!m_shaderType.has_value() || !m_shaderCodeStr.has_value())
			throw std::runtime_error("Could not build shader module without code and type");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_shaderCodeStr.value().size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(m_shaderCodeStr.value().data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vulkanDevice->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!");
		}

		return std::make_shared<VulkanShaderModule>(vulkanDevice, shaderModule, m_shaderType.value());
	}

	VulkanShaderModule::~VulkanShaderModule() = default;

	void VulkanShaderModule::Release() {
		vkDestroyShaderModule(m_device->getLogicalDevice(), m_shaderModule, nullptr);
	}


#pragma endregion VulkanShaderModule }

#pragma region VulkanGraphicsPipeline{

	static std::unordered_map<VertexInputRate, VkVertexInputRate> inputRateMap = {
			{VertexInputRate::PerVertex,   VK_VERTEX_INPUT_RATE_VERTEX},
			{VertexInputRate::PerInstance, VK_VERTEX_INPUT_RATE_INSTANCE},
	};

	static std::unordered_map<VertexFormat, VkFormat> vertexFormatMap = {
			{VertexFormat::Float,  VK_FORMAT_R32_SFLOAT},
			{VertexFormat::Float2, VK_FORMAT_R32G32_SFLOAT},
			{VertexFormat::Float3, VK_FORMAT_R32G32B32_SFLOAT},
			{VertexFormat::Float4, VK_FORMAT_R32G32B32A32_SFLOAT},
			{VertexFormat::Int,    VK_FORMAT_R32_SINT},
			{VertexFormat::Int2,   VK_FORMAT_R32G32_SINT},
			{VertexFormat::Int3,   VK_FORMAT_R32G32B32_SINT},
			{VertexFormat::Int4,   VK_FORMAT_R32G32B32A32_SINT},
	};

	static std::unordered_map<PixelFormat, VkFormat> pixelFormatMap = {
			{PixelFormat::RGBA8_UNORM,      VK_FORMAT_R8G8B8A8_UNORM},
			{PixelFormat::BGRA8_UNORM,      VK_FORMAT_B8G8R8A8_UNORM},
			{PixelFormat::RGBA16_FLOAT,     VK_FORMAT_R16G16B16A16_SFLOAT},
			{PixelFormat::RGBA32_FLOAT,     VK_FORMAT_R32G32B32A32_SFLOAT},
			{PixelFormat::DEPTH32_FLOAT,    VK_FORMAT_D32_SFLOAT},
			{PixelFormat::DEPTH24_STENCIL8, VK_FORMAT_D24_UNORM_S8_UINT},
			{PixelFormat::R8_UNORM,         VK_FORMAT_R8_UNORM},
			{PixelFormat::R16_FLOAT,        VK_FORMAT_R16_SFLOAT},
			{PixelFormat::BC1,              VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
			{PixelFormat::BC3,              VK_FORMAT_BC3_UNORM_BLOCK},
			{PixelFormat::PVRTC,            VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG},
	};

	static std::unordered_map<InputAssemblyTopology, VkPrimitiveTopology> topologyMap = {
			{InputAssemblyTopology::LineList,      VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
			{InputAssemblyTopology::LineStrip,     VK_PRIMITIVE_TOPOLOGY_LINE_STRIP},
			{InputAssemblyTopology::PointList,     VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
			{InputAssemblyTopology::TriangleList,  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
			{InputAssemblyTopology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP},
	};

	static std::unordered_map<BlendFactor, VkBlendFactor> blendFactorMap = {
			{BlendFactor::Zero,                  VK_BLEND_FACTOR_ZERO},
			{BlendFactor::One,                   VK_BLEND_FACTOR_ONE},
			{BlendFactor::SrcColor,              VK_BLEND_FACTOR_SRC_COLOR},
			{BlendFactor::OneMinusSrcColor,      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR},
			{BlendFactor::DstColor,              VK_BLEND_FACTOR_DST_COLOR},
			{BlendFactor::OneMinusDstColor,      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR},
			{BlendFactor::SrcAlpha,              VK_BLEND_FACTOR_SRC_ALPHA},
			{BlendFactor::OneMinusSrcAlpha,      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
			{BlendFactor::DstAlpha,              VK_BLEND_FACTOR_DST_ALPHA},
			{BlendFactor::OneMinusDstAlpha,      VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA},
			{BlendFactor::ConstantColor,         VK_BLEND_FACTOR_CONSTANT_COLOR},
			{BlendFactor::OneMinusConstantColor, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR},
			{BlendFactor::ConstantAlpha,         VK_BLEND_FACTOR_CONSTANT_ALPHA},
			{BlendFactor::OneMinusConstantAlpha, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA},
	};

	static std::unordered_map<BlendOperation, VkBlendOp> blendOpMap = {
			{BlendOperation::Add,             VK_BLEND_OP_ADD},
			{BlendOperation::Subtract,        VK_BLEND_OP_SUBTRACT},
			{BlendOperation::ReverseSubtract, VK_BLEND_OP_REVERSE_SUBTRACT},
			{BlendOperation::Min,             VK_BLEND_OP_MIN},
			{BlendOperation::Max,             VK_BLEND_OP_MAX},
	};

	static std::unordered_map<DepthCompareOperation, VkCompareOp> depthCompareOpMap = {
			{DepthCompareOperation::Never,        VK_COMPARE_OP_NEVER},
			{DepthCompareOperation::Less,         VK_COMPARE_OP_LESS},
			{DepthCompareOperation::LessEqual,    VK_COMPARE_OP_LESS_OR_EQUAL},
			{DepthCompareOperation::Equal,        VK_COMPARE_OP_EQUAL},
			{DepthCompareOperation::GreaterEqual, VK_COMPARE_OP_GREATER_OR_EQUAL},
			{DepthCompareOperation::Greater,      VK_COMPARE_OP_GREATER},
			{DepthCompareOperation::NotEqual,     VK_COMPARE_OP_NOT_EQUAL},
			{DepthCompareOperation::AllTime,      VK_COMPARE_OP_ALWAYS},
	};

	static std::unordered_map<StencilOpState, VkStencilOp> stencilCompareOpMap = {
			{StencilOpState::Zero,              VK_STENCIL_OP_ZERO},
			{StencilOpState::DecrementAndClamp, VK_STENCIL_OP_DECREMENT_AND_CLAMP},
			{StencilOpState::DecrementAndWrap,  VK_STENCIL_OP_DECREMENT_AND_WRAP},
			{StencilOpState::IncrementAndClamp, VK_STENCIL_OP_INCREMENT_AND_CLAMP},
			{StencilOpState::IncrementAndWrap,  VK_STENCIL_OP_INCREMENT_AND_WRAP},
			{StencilOpState::Invert,            VK_STENCIL_OP_INVERT},
			{StencilOpState::Keep,              VK_STENCIL_OP_KEEP},
			{StencilOpState::Replace,           VK_STENCIL_OP_REPLACE},
	};

	VkSampleCountFlagBits convertToVkSampleCountFlagBits(uint8_t sampleCount) {
		switch (sampleCount) {
			case 2:
				return VK_SAMPLE_COUNT_2_BIT;
			case 4:
				return VK_SAMPLE_COUNT_4_BIT;
			case 8:
				return VK_SAMPLE_COUNT_8_BIT;
			case 16:
				return VK_SAMPLE_COUNT_16_BIT;
			case 32:
				return VK_SAMPLE_COUNT_32_BIT;
			case 64:
				return VK_SAMPLE_COUNT_64_BIT;
			default:
				return VK_SAMPLE_COUNT_1_BIT;
		}
	}

	VulkanGraphicsPipelineBuilder::VulkanGraphicsPipelineBuilder() = default;

	VulkanGraphicsPipelineBuilder::~VulkanGraphicsPipelineBuilder() = default;

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::SetShaderStage(Graphics::ShaderStage shaderStage) {

		auto vulkanVertexShaderModule = reinterpret_cast<std::shared_ptr<VulkanShaderModule>&>(shaderStage.vertexShaderModule);
		auto vulkanFragmentShaderModule = reinterpret_cast<std::shared_ptr<VulkanShaderModule>&>(shaderStage.fragmentShaderModule);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vulkanVertexShaderModule->getShaderModule();
		vertShaderStageInfo.pName = shaderStage.mainFunctionName.c_str();

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = vulkanFragmentShaderModule->getShaderModule();
		fragShaderStageInfo.pName = shaderStage.mainFunctionName.c_str();

		m_shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();
		m_shaderStages.push_back(vertShaderStageInfo);
		m_shaderStages.push_back(fragShaderStageInfo);
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);
		return this;
	}

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::SetVertexInput(Graphics::VertexInput vertexInput) {

		m_bindingDescriptions = std::vector<VkVertexInputBindingDescription>();
		for (auto bindingDescData: vertexInput.bindingDescriptions) {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = bindingDescData.bindingIndex;
			bindingDescription.stride = bindingDescData.stride;
			bindingDescription.inputRate = inputRateMap[bindingDescData.inputRate];
			m_bindingDescriptions.push_back(bindingDescription);
		}
		m_attributeDescriptions = std::vector<VkVertexInputAttributeDescription>();
		for (auto attributeDescData: vertexInput.attributeDescriptions) {
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.binding = attributeDescription.binding;
			attributeDescription.location = attributeDescData.attributeLocation;
			attributeDescription.format = vertexFormatMap[attributeDescData.vertexFormat];
			attributeDescription.offset = attributeDescData.offset;
			m_attributeDescriptions.push_back(attributeDescription);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = m_bindingDescriptions.size();
		vertexInputInfo.pVertexBindingDescriptions = m_bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = m_attributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();

		m_vertexInput = std::make_optional(vertexInputInfo);
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetInputAssemblyTopology(Graphics::InputAssemblyTopology topology) {
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = topologyMap[topology];
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		m_inputAssembly = std::make_optional(inputAssembly);

		return this;
	}

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::SetMultisamplingCount(uint16_t multisamplingCount) {

		VkSampleCountFlagBits sampleCountFlagBits = convertToVkSampleCountFlagBits(multisamplingCount);

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = sampleCountFlagBits;
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.minSampleShading = 0.2f;

		m_multisampling = std::make_optional(multisampling);
		return this;
	}

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::BindDescriptorLayouts(
			std::vector<std::shared_ptr<JarDescriptorLayout>> descriptorLayouts) {

		for (auto& descriptorLayout: descriptorLayouts) {
			auto vulkanDescriptorLayout = reinterpret_cast<std::shared_ptr<VulkanDescriptorLayout>&>(descriptorLayout);
			m_descriptorSetLayouts.push_back(vulkanDescriptorLayout->getDescriptorSetLayout());
		}
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetColorBlendAttachments(Graphics::ColorBlendAttachment colorBlendAttachment) {
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.colorBlendOp = blendOpMap[colorBlendAttachment.rgbBlendOperation];
		colorBlendAttachmentState.alphaBlendOp = blendOpMap[colorBlendAttachment.alphaBlendOperation];
		colorBlendAttachmentState.blendEnable = static_cast<VkBool32>(colorBlendAttachment.blendingEnabled);
		colorBlendAttachmentState.srcColorBlendFactor = blendFactorMap[colorBlendAttachment.sourceRgbBlendFactor];
		colorBlendAttachmentState.srcAlphaBlendFactor = blendFactorMap[colorBlendAttachment.sourceAlphaBlendFactor];
		colorBlendAttachmentState.dstColorBlendFactor = blendFactorMap[colorBlendAttachment.destinationRgbBlendFactor];
		colorBlendAttachmentState.dstAlphaBlendFactor = blendFactorMap[colorBlendAttachment.destinationAlphaBlendFactor];
		colorBlendAttachmentState.colorWriteMask = convertToColorComponentFlagBits(colorBlendAttachment.colorWriteMask);

		m_colorBlendAttachmentStates = std::vector<VkPipelineColorBlendAttachmentState>();
		m_colorBlendAttachmentStates.push_back(colorBlendAttachmentState);

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = m_colorBlendAttachmentStates.size();
		colorBlending.pAttachments = m_colorBlendAttachmentStates.data();

		m_colorBlend = std::make_optional(colorBlending);
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetDepthStencilState(Graphics::DepthStencilState depthStencilState) {

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.depthTestEnable = static_cast<VkBool32>(depthStencilState.depthTestEnable);
		depthStencilStateCreateInfo.depthCompareOp = depthCompareOpMap[depthStencilState.depthCompareOp];
		depthStencilStateCreateInfo.depthWriteEnable = static_cast<VkBool32>(depthStencilState.depthWriteEnable);
		depthStencilStateCreateInfo.stencilTestEnable = static_cast<VkBool32>(depthStencilState.stencilTestEnable);
		if (depthStencilState.stencilTestEnable) {
			depthStencilStateCreateInfo.front.failOp = stencilCompareOpMap[depthStencilState.stencilOpState];
			depthStencilStateCreateInfo.front.passOp = stencilCompareOpMap[depthStencilState.stencilOpState];
			depthStencilStateCreateInfo.front.depthFailOp = stencilCompareOpMap[depthStencilState.stencilOpState];
			depthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilStateCreateInfo.front.compareMask = 0xFF;
			depthStencilStateCreateInfo.front.writeMask = 0xFF;
			depthStencilStateCreateInfo.front.reference = 1;

			depthStencilStateCreateInfo.back.failOp = stencilCompareOpMap[depthStencilState.stencilOpState];
			depthStencilStateCreateInfo.back.passOp = stencilCompareOpMap[depthStencilState.stencilOpState];
			depthStencilStateCreateInfo.back.depthFailOp = stencilCompareOpMap[depthStencilState.stencilOpState];
			depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilStateCreateInfo.back.compareMask = 0xFF;
			depthStencilStateCreateInfo.back.writeMask = 0xFF;
			depthStencilStateCreateInfo.back.reference = 1;
		}
		m_depthStencil = std::make_optional(depthStencilStateCreateInfo);
		return this;
	}

	std::shared_ptr<JarPipeline> VulkanGraphicsPipelineBuilder::Build(std::shared_ptr<JarDevice> device) {

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
		pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(vulkanDevice->getLogicalDevice(), &pipelineLayoutCreateInfo, nullptr,
		                           &m_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
		std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
		                                             VK_DYNAMIC_STATE_DEPTH_BIAS};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

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
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_TRUE;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = m_shaderStages.size();
		pipelineInfo.pStages = m_shaderStages.data();
		pipelineInfo.pVertexInputState = &m_vertexInput.value();
		pipelineInfo.pInputAssemblyState = &m_inputAssembly.value();
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &m_multisampling.value();
		if (m_depthStencil.has_value()) {
			pipelineInfo.pDepthStencilState = &m_depthStencil.value();
		}
		pipelineInfo.pColorBlendState = &m_colorBlend.value();
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass->getRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		auto result = vkCreateGraphicsPipelines(vulkanDevice->getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
		                                        nullptr,
		                                        &m_pipeline);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline");
		}

		return std::make_shared<VulkanGraphicsPipeline>(vulkanDevice, m_pipelineLayout, m_pipeline, m_renderPass);
	}

	VkColorComponentFlags
	VulkanGraphicsPipelineBuilder::convertToColorComponentFlagBits(Graphics::ColorWriteMask colorWriteMask) {
		VkColorComponentFlags flagBits = 0;
		auto maskValue = static_cast<std::underlying_type<ColorWriteMask>::type>(colorWriteMask);
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Red))
			flagBits |= VK_COLOR_COMPONENT_R_BIT;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Green))
			flagBits |= VK_COLOR_COMPONENT_G_BIT;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Blue))
			flagBits |= VK_COLOR_COMPONENT_B_BIT;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Alpha))
			flagBits |= VK_COLOR_COMPONENT_A_BIT;
		return flagBits;
	}


	VulkanGraphicsPipeline::~VulkanGraphicsPipeline() = default;

	std::shared_ptr<JarRenderPass> VulkanGraphicsPipeline::GetRenderPass() {
		return m_renderPass;
	}

	void VulkanGraphicsPipeline::Release() {

		m_renderPass->Release();
		vkDestroyPipelineLayout(m_device->getLogicalDevice(), m_pipelineLayout, nullptr);
		vkDestroyPipeline(m_device->getLogicalDevice(), m_graphicsPipeline, nullptr);
	}

#pragma endregion VulkanGraphicsPipeline }

#pragma region VulkanFramebuffer{

	VulkanFramebuffer::~VulkanFramebuffer() = default;

	void VulkanFramebuffer::Release() {
		vkDestroyFramebuffer(m_device->getLogicalDevice(), m_framebuffer, nullptr);
	}

	void VulkanFramebuffer::CreateFramebuffer(std::shared_ptr<VulkanDevice> device, VkRenderPass renderPass,
	                                          VkImageView swapchainImageView, VkImageView depthImageView,
	                                          VkImageView colorImageView) {

		m_device = device;

		std::vector<VkImageView> attachments = std::vector<VkImageView>();
		attachments.push_back(colorImageView);
		attachments.push_back(depthImageView);
		attachments.push_back(swapchainImageView);

		m_framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		m_framebufferCreateInfo.renderPass = renderPass;
		m_framebufferCreateInfo.attachmentCount = attachments.size();
		m_framebufferCreateInfo.pAttachments = attachments.data();
		m_framebufferCreateInfo.width = m_framebufferExtent.width;
		m_framebufferCreateInfo.height = m_framebufferExtent.height;
		m_framebufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(m_device->getLogicalDevice(), &m_framebufferCreateInfo, nullptr, &m_framebuffer) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}

	void VulkanFramebuffer::RecreateFramebuffer(uint32_t width, uint32_t height, VkImageView swapchainImageView,
	                                            VkImageView depthImageView, VkImageView colorImageView) {

		std::vector<VkImageView> attachments = std::vector<VkImageView>();
		attachments.push_back(colorImageView);
		attachments.push_back(depthImageView);
		attachments.push_back(swapchainImageView);

		m_framebufferExtent.width = width;
		m_framebufferExtent.height = height;
		m_framebufferCreateInfo.width = m_framebufferExtent.width;
		m_framebufferCreateInfo.height = m_framebufferExtent.height;
		m_framebufferCreateInfo.pAttachments = attachments.data();
		m_framebufferCreateInfo.attachmentCount = attachments.size();

		if (vkCreateFramebuffer(m_device->getLogicalDevice(), &m_framebufferCreateInfo, nullptr, &m_framebuffer) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}

#pragma endregion VulkanFramebuffer }

#pragma region VulkanRenderPass{

	static std::unordered_map<ImageFormat, VkFormat> imageFormatMap = {
			{ImageFormat::B8G8R8A8_UNORM,    VK_FORMAT_B8G8R8A8_UNORM},
			{ImageFormat::D32_SFLOAT,        VK_FORMAT_D32_SFLOAT},
			{ImageFormat::D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
	};

	static std::unordered_map<StoreOp, VkAttachmentStoreOp> storeOpMap = {
			{StoreOp::Store,    VK_ATTACHMENT_STORE_OP_STORE},
			{StoreOp::DontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE},
	};

	static std::unordered_map<LoadOp, VkAttachmentLoadOp> loadOpMap = {
			{LoadOp::Load,     VK_ATTACHMENT_LOAD_OP_LOAD},
			{LoadOp::DontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE},
			{LoadOp::Clear,    VK_ATTACHMENT_LOAD_OP_CLEAR},
	};

	VulkanRenderPassBuilder::~VulkanRenderPassBuilder() = default;

	VulkanRenderPassBuilder* VulkanRenderPassBuilder::AddColorAttachment(Graphics::ColorAttachment colorAttachment) {
		VkAttachmentDescription colorAttachmentDesc{};
		colorAttachmentDesc.format = imageFormatMap[colorAttachment.imageFormat];
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = loadOpMap[colorAttachment.loadOp];
		colorAttachmentDesc.storeOp = storeOpMap[colorAttachment.storeOp];
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		m_colorAttachment = std::make_optional(colorAttachmentDesc);

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_colorAttachmentRef = std::make_optional(colorAttachmentRef);

		return this;
	}

	VulkanRenderPassBuilder* VulkanRenderPassBuilder::AddDepthStencilAttachment(
			Graphics::DepthAttachment depthStencilAttachment) {
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = imageFormatMap[depthStencilAttachment.Format];
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = loadOpMap[depthStencilAttachment.DepthLoadOp];
		depthAttachment.storeOp = storeOpMap[depthStencilAttachment.DepthStoreOp];
		if (depthStencilAttachment.Stencil.has_value()) {
			depthAttachment.stencilLoadOp = loadOpMap[depthStencilAttachment.Stencil.value().StencilLoadOp];
			depthAttachment.stencilStoreOp = storeOpMap[depthStencilAttachment.Stencil.value().StencilStoreOp];
		}
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_depthStencilAttachment = std::make_optional(depthAttachment);

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_depthStencilAttachmentRef = std::make_optional(depthAttachmentRef);
		return this;
	}

	VulkanRenderPassBuilder* VulkanRenderPassBuilder::SetMultisamplingCount(uint8_t multisamplingCount) {
		m_multisamplingCount = std::make_optional(convertToVkSampleCountFlagBits(multisamplingCount));
		return this;
	}

	std::shared_ptr<JarRenderPass>
	VulkanRenderPassBuilder::Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) {

		if (!m_colorAttachment.has_value())
			throw std::runtime_error("Could not create render pass without color attachment set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		auto vulkanSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

		if (!m_multisamplingCount.has_value())
			throw std::runtime_error("Could not create render pass without multisampling count set!");

		auto msaaSamples = m_multisamplingCount.value();
		m_colorAttachment.value().samples = msaaSamples;

		std::vector<VkAttachmentDescription> attachments = std::vector<VkAttachmentDescription>();
		attachments.push_back(m_colorAttachment.value());

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &m_colorAttachmentRef.value();

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = m_colorAttachment.value().format;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		if (m_depthStencilAttachment.has_value()) {
			m_depthStencilAttachment.value().samples = msaaSamples;
			attachments.push_back(m_depthStencilAttachment.value());
			subpass.pDepthStencilAttachment = &m_depthStencilAttachmentRef.value();

			dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		attachments.push_back(colorAttachmentResolve);

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass renderPass;
		if (vkCreateRenderPass(vulkanDevice->getLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		auto vulkanRenderPassFramebuffers = std::make_unique<VulkanRenderPassFramebuffers>();
		vulkanRenderPassFramebuffers->CreateRenderPassFramebuffers(vulkanDevice, vulkanSurface, renderPass, msaaSamples);

		auto vulkanRenderPass = std::make_shared<VulkanRenderPass>(vulkanDevice, renderPass, vulkanRenderPassFramebuffers);
		return vulkanRenderPass;
	}

	void VulkanRenderPassFramebuffers::CreateRenderPassFramebuffers(std::shared_ptr<VulkanDevice> device,
	                                                                std::shared_ptr<VulkanSurface> surface,
	                                                                VkRenderPass renderPass,
	                                                                VkSampleCountFlagBits multisamplingCount) {
		m_device = device;
		auto imageExtent = surface->getSwapchain()->getSwapchainImageExtent();
		auto colorFormat = surface->getSwapchain()->getSwapchainImageFormat();
		auto depthFormat = surface->getSwapchain()->findDepthFormat();

		m_msaaSamples = multisamplingCount;

		createColorResources(imageExtent, colorFormat);
		createDepthResources(imageExtent, depthFormat);

		for (const auto& m_swapchainImageView: surface->getSwapchain()->getSwapchainImageViews()) {
			auto framebuffer = std::make_shared<VulkanFramebuffer>(imageExtent);
			framebuffer->CreateFramebuffer(m_device, renderPass, m_swapchainImageView, m_depthImageView,
			                               m_colorImageView);
			m_framebuffers.push_back(framebuffer);
		}
	}

	std::shared_ptr<VulkanFramebuffer> VulkanRenderPassFramebuffers::GetFramebuffer(uint32_t index) {
		return m_framebuffers[index];
	}

	void VulkanRenderPassFramebuffers::Release() {
		for (const auto& framebuffer: m_framebuffers) {
			framebuffer->Release();
		}

		vkDestroyImageView(m_device->getLogicalDevice(), m_colorImageView, nullptr);
		vkDestroyImage(m_device->getLogicalDevice(), m_colorImage, nullptr);
		vkFreeMemory(m_device->getLogicalDevice(), m_colorImageMemory, nullptr);

		vkDestroyImageView(m_device->getLogicalDevice(), m_depthImageView, nullptr);
		vkFreeMemory(m_device->getLogicalDevice(), m_depthImageMemory, nullptr);
		vkDestroyImage(m_device->getLogicalDevice(), m_depthImage, nullptr);
	}

	void VulkanRenderPassFramebuffers::RecreateFramebuffers(VkExtent2D swapchainExtent,
	                                                        std::shared_ptr<VulkanSurface> surface) {
		auto swapchainImageViews = surface->getSwapchain()->getSwapchainImageViews();
		for (int i = 0; i < m_framebuffers.size(); ++i) {
			m_framebuffers[i]->RecreateFramebuffer(swapchainExtent.width, swapchainExtent.height,
			                                       swapchainImageViews[i], m_depthImageView, m_colorImageView);
		}
	}

	void VulkanRenderPassFramebuffers::createDepthResources(VkExtent2D imageExtent, VkFormat depthFormat) {
		VulkanImageBuilder::createImage(m_device, imageExtent.width, imageExtent.height, 1, m_msaaSamples,
		                                depthFormat, VK_IMAGE_TILING_OPTIMAL,
		                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory);
		VulkanImageBuilder::createImageView(m_device, m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT,
		                                    &m_depthImageView, 1);
	}

	void VulkanRenderPassFramebuffers::createColorResources(VkExtent2D imageExtent, VkFormat colorFormat) {
		VulkanImageBuilder::createImage(m_device, imageExtent.width, imageExtent.height, 1, m_msaaSamples,
		                                colorFormat, VK_IMAGE_TILING_OPTIMAL,
		                                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_colorImage, m_colorImageMemory);
		VulkanImageBuilder::createImageView(m_device, m_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT,
		                                    &m_colorImageView, 1);

	}

	VulkanRenderPass::~VulkanRenderPass() = default;

	void VulkanRenderPass::Release() {
		m_framebuffers->Release();
		vkDestroyRenderPass(m_device->getLogicalDevice(), m_renderPass, nullptr);
	}

#pragma endregion VulkanRenderPass }

#pragma region VulkanImage{

	VulkanImageBuilder::~VulkanImageBuilder() = default;

	VulkanImageBuilder* VulkanImageBuilder::SetPixelFormat(Graphics::PixelFormat format) {
		m_pixelFormat = std::make_optional(format);
		return this;
	}

	VulkanImageBuilder* VulkanImageBuilder::SetImagePath(std::string imagePath) {
		m_imagePath = std::make_optional(imagePath);
		return this;
	}

	VulkanImageBuilder* VulkanImageBuilder::EnableMipMaps(bool enabled) {
		m_enableMipMapping = enabled;
		return this;
	}

	std::shared_ptr<JarImage> VulkanImageBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(m_imagePath.value().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		if (!pixels) {
			throw std::runtime_error("Failed to load texture image!");
		}


		auto mipLevels = 1;
		if (m_enableMipMapping)
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
		auto imageExtent = VkExtent2D{static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)};

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanBufferBuilder::createBuffer(vulkanDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		                                  stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory);

		stbi_image_free(pixels);

		auto vkFormat = pixelFormatMap[m_pixelFormat.value()];

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		createImage(vulkanDevice, texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, vkFormat,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

		transitionImageLayout(vulkanDevice, textureImage, vkFormat,
		                      VK_IMAGE_LAYOUT_UNDEFINED,
		                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
		copyBufferToImage(vulkanDevice, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth),
		                  static_cast<uint32_t>(texHeight));


		//transitionImageLayout(vulkanDevice, textureImage, vkFormat,
		//VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		//		mipLevels);

		vkDestroyBuffer(vulkanDevice->getLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(vulkanDevice->getLogicalDevice(), stagingBufferMemory, nullptr);

		generateMipMaps(vulkanDevice, textureImage, vkFormat, imageExtent.width, imageExtent.height, mipLevels);


		VkImageView textureImageView;
		createImageView(vulkanDevice, textureImage, vkFormat, VK_IMAGE_ASPECT_COLOR_BIT, &textureImageView, mipLevels);

		VkSampler sampler;
		createSampler(vulkanDevice, sampler, mipLevels);

		return std::make_shared<VulkanImage>(vulkanDevice, textureImage, textureImageMemory, textureImageView,
		                                     vkFormat, imageExtent, sampler, m_nextImageId++);
	}

	void
	VulkanImageBuilder::createImage(std::shared_ptr<VulkanDevice>& vulkanDevice, uint32_t texWidth, uint32_t texHeight,
	                                uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
	                                VkImageTiling tiling, VkImageUsageFlags usage,
	                                VkMemoryPropertyFlags properties, VkImage& image,
	                                VkDeviceMemory& imageMemory) {

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texWidth);
		imageInfo.extent.height = static_cast<uint32_t>(texHeight);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;
		imageInfo.flags = 0;

		if (vkCreateImage(vulkanDevice->getLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vulkanDevice->getLogicalDevice(), image, &memRequirements);
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanBufferBuilder::findMemoryType(vulkanDevice->getPhysicalDevice(),
		                                                                memRequirements.memoryTypeBits,
		                                                                properties);
		if (vkAllocateMemory(vulkanDevice->getLogicalDevice(), &allocInfo, nullptr, &imageMemory) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate image memory!");
		}
		vkBindImageMemory(vulkanDevice->getLogicalDevice(), image, imageMemory, 0);
	}

	void
	VulkanImageBuilder::createImageView(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat format,
	                                    VkImageAspectFlags aspectFlags, VkImageView* imageView, uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(vulkanDevice->getLogicalDevice(), &viewInfo, nullptr, imageView) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture image view!");
		}
	}

	void VulkanImageBuilder::transitionImageLayout(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image,
	                                               VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
	                                               uint32_t mipLevels) {
		auto commandQueue = m_backend->getStagingCommandQueue();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(vulkanDevice, commandQueue);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationFlags;
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		           newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationFlags, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		VulkanCommandBuffer::EndSingleTimeRecording(vulkanDevice, commandBuffer, commandQueue);
	}

	void VulkanImageBuilder::copyBufferToImage(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer buffer,
	                                           VkImage image, uint32_t width, uint32_t height) {
		auto commandQueue = m_backend->getStagingCommandQueue();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(vulkanDevice, commandQueue);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {width, height, 1};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		VulkanCommandBuffer::EndSingleTimeRecording(vulkanDevice, commandBuffer, commandQueue);

	}

	void VulkanImageBuilder::generateMipMaps(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image,
	                                         VkFormat imageFormat, int32_t texWidth, int32_t texHeight,
	                                         uint32_t mipLevels) {

		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(vulkanDevice->getPhysicalDevice(), imageFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("Texture image format does not support linear blitting!");
		}

		auto commandQueue = m_backend->getStagingCommandQueue();
		VkCommandBuffer commandBuffer = VulkanCommandBuffer::StartSingleTimeRecording(vulkanDevice, commandQueue);

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
			                     nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = {0, 0, 0};
			blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = {0, 0, 0};
			blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
			               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			                     0, 0, nullptr, 0, nullptr, 1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		                     0, nullptr, 0, nullptr, 1, &barrier);

		VulkanCommandBuffer::EndSingleTimeRecording(vulkanDevice, commandBuffer, commandQueue);
	}

	void VulkanImageBuilder::createSampler(std::shared_ptr<VulkanDevice>& vulkanDevice, VkSampler& sampler,
	                                       uint32_t mipLevels) {

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(vulkanDevice->getPhysicalDevice(), &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(mipLevels);

		if (vkCreateSampler(vulkanDevice->getLogicalDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture sampler!");
		}
	}

	VulkanImage::~VulkanImage() = default;

	void VulkanImage::Release() {
		vkDestroySampler(m_device->getLogicalDevice(), m_sampler, nullptr);
		vkDestroyImageView(m_device->getLogicalDevice(), m_imageView, nullptr);
		vkDestroyImage(m_device->getLogicalDevice(), m_image, nullptr);
		vkFreeMemory(m_device->getLogicalDevice(), m_imageMemory, nullptr);
	}

#pragma endregion VulkanImage }

}
#endif
