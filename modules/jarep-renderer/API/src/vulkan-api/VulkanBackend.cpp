//
// Created by Sebastian Borsch on 24.10.23.
//

#if defined(__linux__) or defined(_WIN32)

#include "VulkanBackend.hpp"

namespace Graphics::Vulkan {

	extern "C" Graphics::Backend* CreateVulkanBackend(const char* const* extensions, size_t count) {
		return new Graphics::Vulkan::VulkanBackend(extensions, count);
	}

	VulkanBackend::VulkanBackend(const char* const* extensions, size_t count) {
		extensionNames = std::vector<const char*>(extensions, extensions + count);
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
		return new VulkanRenderPassBuilder([this]() { return this->getStagingCommandQueue(); });
	}

	JarCommandQueueBuilder* VulkanBackend::InitCommandQueueBuilder() {
		return new VulkanCommandQueueBuilder();
	};

	JarBufferBuilder* VulkanBackend::InitBufferBuilder() {
		return new VulkanBufferBuilder([this]() { return this->getStagingCommandQueue(); },
		                               [this]() { return this->onRegisterNewBuffer(); },
		                               [this]() { return this->onDestroyBuffer(); });
	}

	JarImageBuilder* VulkanBackend::InitImageBuilder() {
		return new VulkanImageBuilder([this]() { return this->getStagingCommandQueue(); });
	}

	JarPipelineBuilder* VulkanBackend::InitPipelineBuilder() {
		return new VulkanGraphicsPipelineBuilder();
	}

	JarDescriptorBuilder* VulkanBackend::InitDescriptorBuilder() {
		return new VulkanDescriptorBuilder();
	}

	JarRenderTargetBuilder* VulkanBackend::InitRenderTargetBuilder() {
		return new VulkanRenderTargetBuilder();
	}

	JarFramebufferBuilder* VulkanBackend::InitFramebufferBuilder() {
		return new VulkanFramebufferBuilder();
	}

	JarImageBufferBuilder* VulkanBackend::InitImageBufferBuilder() {
		return new VulkanImageBufferBuilder();
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


}
#endif
