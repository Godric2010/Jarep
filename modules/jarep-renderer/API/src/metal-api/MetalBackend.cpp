//
// Created by Sebastian Borsch on 24.10.23.
//
#include <iostream>
#include <utility>

#if defined(__APPLE__)

#include "MetalBackend.hpp"


namespace Graphics::Metal {

	MetalBackend::MetalBackend() = default;

	MetalBackend::~MetalBackend() = default;

	BackendType MetalBackend::GetType() {
		return BackendType::Metal;
	}

	std::shared_ptr<JarSurface> MetalBackend::CreateSurface(NativeWindowHandleProvider* windowHandleProvider) {
		auto metalSurface = std::make_shared<MetalSurface>();
		metalSurface->CreateFromNativeWindowProvider(windowHandleProvider);
		return metalSurface;
	}

	std::shared_ptr<JarDevice> MetalBackend::CreateDevice(std::shared_ptr<JarSurface>& surface) {
		auto metalDevice = std::make_shared<MetalDevice>();
		metalDevice->Initialize();

		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);
		metalSurface->FinalizeSurface(metalDevice);

		return metalDevice;
	}

	JarShaderModuleBuilder* MetalBackend::InitShaderModuleBuilder() {
		return new MetalShaderLibraryBuilder();
	}

	JarRenderPassBuilder* MetalBackend::InitRenderPassBuilder() {
		return new MetalRenderPassBuilder();
	}

	JarCommandQueueBuilder* MetalBackend::InitCommandQueueBuilder() {
		return new MetalCommandQueueBuilder();
	}

	JarBufferBuilder* MetalBackend::InitBufferBuilder() {
		return new MetalBufferBuilder();
	}

	JarPipelineBuilder* MetalBackend::InitPipelineBuilder() {
		return new MetalPipelineBuilder();
	}

	JarImageBuilder* MetalBackend::InitImageBuilder() {
		return new MetalImageBuilder();
	}

	JarDescriptorBuilder* MetalBackend::InitDescriptorBuilder() {
		return new MetalDescriptorBuilder();
	}

	JarRenderTargetBuilder* MetalBackend::InitRenderTargetBuilder() {
		return new MetalRenderTargetBuilder();
	}

	JarFramebufferBuilder* MetalBackend::InitFramebufferBuilder() {
		return new MetalFramebufferBuilder();
	}

	JarImageBufferBuilder* MetalBackend::InitImageBufferBuilder() {
		return new MetalImageBufferBuilder();
	}

	extern "C" Backend* CreateMetalBackend() {
		return new MetalBackend();
	}
}
#endif
