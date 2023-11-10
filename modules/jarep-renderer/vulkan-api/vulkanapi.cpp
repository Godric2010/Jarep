//
// Created by Sebastian Borsch on 24.10.23.
//
#if defined(__linux__) or defined(_WIN32)

#include "vulkanapi.hpp"

namespace Graphics::Vulkan {
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
	}

	void VulkanAPI::CreateSurface(NativeWindowHandleProvider *nativeWindowHandle) {
		switch (nativeWindowHandle->getWindowSystem()) {
			case Cocoa:
				throw std::runtime_error("Using a cocoa window with vulkan is not allowed!");
			case Win32: {
				/*VkWin32SurfaceCreateInfoKHR createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
				createInfo.hwnd = reinterpret_cast<HWND>(nativeWindowHandle.getNativeWindowHandle());
				createInfo.hInstance = GetModuleHandle(nullptr);

				if(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface)) {
					throw std::runtime_error("Failed to create Win32 surface");
				}*/
				return;
			}
			case Wayland: {
				auto waylandWindowHandle = dynamic_cast<WaylandWindowHandleProvider *>(nativeWindowHandle);

				VkWaylandSurfaceCreateInfoKHR waylandCreateInfo = {};
				waylandCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
				waylandCreateInfo.display = waylandWindowHandle->getDisplay();
				waylandCreateInfo.surface = waylandWindowHandle->getWindowHandle();

				if (vkCreateWaylandSurfaceKHR(instance, &waylandCreateInfo, nullptr, &surface) != VK_SUCCESS) {
					throw std::runtime_error("Failed to create Wayland surface.");
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
	}

	void VulkanAPI::CreateVertexBuffer() {
	}

	void VulkanAPI::CreateShaders() {
	}

	void VulkanAPI::CreateCommandQueue() {
	}

	void VulkanAPI::CreateGraphicsPipeline() {
	}

	void VulkanAPI::RecordCommandBuffer() {
	}

	void VulkanAPI::Shutdown() {
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
		return indices.isComplete() && extensionSupported;
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
}
#endif
