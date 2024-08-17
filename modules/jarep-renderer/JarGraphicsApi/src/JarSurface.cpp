//
// Created by sebastian on 15.08.24.
//

#include "../include/JarSurface.hpp"

#if defined(PLATFORM_VULKAN)
	#include "../platforms/vulkan/include/VulkanSurface.hpp"
#elif defined(PLATFORM_METAL)
	#include "../platforms/metal/include/MetalSurface.hpp"
#endif

std::shared_ptr<JarSurface> JarSurface::Create(std::shared_ptr<JarBackend>& backend, NativeWindowHandleProvider* windowHandle) {
#if defined(PLATFORM_VULKAN)
	return std::make_shared<VulkanSurface>(backend, windowHandle);
#elif defined(PLATFORM_METAL)
	return std::make_shared<MetalSurface>(backend, windowHandle;)
#endif
}