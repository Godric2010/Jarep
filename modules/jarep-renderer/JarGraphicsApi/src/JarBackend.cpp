//
// Created by sebastian on 03.08.24.
//

#include "../include/JarBackend.hpp"


#if defined(PLATFORM_VULKAN)
	#include "../platforms/vulkan/include/VulkanBackend.hpp"
#elif defined(PLATFORM_APPLE)
	#include "metal/MetalBackend.hpp"
#endif

std::shared_ptr<JarBackend> JarBackend::Create(const std::vector<const char*>& extensionNames) {
#if defined(PLATFORM_VULKAN)
	return std::make_shared<VulkanBackend>(extensionNames);
#elif defined(PLATFORM_APPLE)
	return std::make_shared<MetalBackend>();
#else
	throw std::runtime_error("Unsupported platform")
#endif
}