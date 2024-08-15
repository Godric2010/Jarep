//
// Created by sebastian on 03.08.24.
//

//#include "include/JarBackend.hpp"
//
//#if defined(PLATFORM_VULKAN)
//	#include "Vulkan/VulkanBackend.hpp"
//#elif defined(PLATFORM_APPLE)
//	#include "metal/MetalBackend.hpp"
//#endif
//
//std::shared_ptr<JarBackend> JarBackend::Create(const char* const* extensions, size_t count) {
//#if defined(PLATFORM_VULKAN)
//	return std::make_shared<VulkanBackend>(extensions, count);
//#elif defined(PLATFORM_APPLE)
//	return std::make_shared<MetalBackend>();
//#else
//	throw std::runtime_error("Unsupported platform")
//#endif
//}