//
// Created by sebastian on 15.08.24.
//

#include "JarDevice.hpp"
#if defined(PLATFORM_VULKAN)
	#include "../platforms/vulkan/include/VulkanDevice.hpp"
#elif defined(PLATFORM_APPLE)
	#include "../platforms/metal/include/MetalDevice.hpp"
#endif

std::shared_ptr<JarDevice> JarDevice::Create(const std::shared_ptr<JarSurface>& surface) {
#if defined(PLATFORM_VULKAN)
	return std::make_shared<VulkanDevice>(surface);
#elif defined(PLATFORM_APPLE)
	return std::make_shared<MetalDevice>(surface);
#else
	throw std::runtime_error("Unsupported platform")
#endif
}