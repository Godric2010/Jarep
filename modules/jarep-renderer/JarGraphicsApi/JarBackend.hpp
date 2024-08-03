//
// Created by sebastian on 03.08.24.
//
#pragma once

#include <memory>

class JarBackend {
	public:
	virtual ~JarBackend() = default;

	static std::shared_ptr<JarBackend> Create(const char* const* extensions, size_t count);

	protected:
	JarBackend() = default;
};

#if defined(PLATFORM_VULKAN)
class VulkanBackend;
using DefaultBackend = VulkanBackend;
#elif defined(PLATFORM_METAL)
class MetalBackend;
using DefaultBackend = MetalBackend;
//#else
//	#error "Unsupported platform"
#endif
