//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALBACKEND_HPP
#define JAREP_METALBACKEND_HPP

#if defined(__APPLE__)

#include "IRenderAPI.hpp"
#include "MetalSurface.hpp"
#include "MetalShaderModuleBuilder.hpp"
#include "MetalRenderPassBuilder.hpp"
#include "MetalCommandQueueBuilder.hpp"
#include "MetalBufferBuilder.hpp"
#include "MetalImageBuilder.hpp"
#include "MetalDescriptorBuilder.hpp"
#include "MetalPipelineBuilder.hpp"

namespace Graphics::Metal {

	class MetalBackend final : public Backend {
		public:
			MetalBackend();

			~MetalBackend() override;

			BackendType GetType() override;

			std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* windowHandleProvider) override;

			std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& m_surface) override;

			JarShaderModuleBuilder* InitShaderModuleBuilder() override;

			JarRenderPassBuilder* InitRenderPassBuilder() override;

			JarCommandQueueBuilder* InitCommandQueueBuilder() override;

			JarBufferBuilder* InitBufferBuilder() override;

			JarImageBuilder* InitImageBuilder() override;

			JarPipelineBuilder* InitPipelineBuilder() override;

			JarDescriptorBuilder* InitDescriptorBuilder() override;
	};

	extern "C" Backend* CreateMetalBackend() {
		return new MetalBackend();
	}
}
#endif
#endif //JAREP_METALBACKEND_HPP
