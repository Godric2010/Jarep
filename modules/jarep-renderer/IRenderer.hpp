//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

#include "NativeWindowHandleProvider.hpp"
#include <memory>
#include <string>
namespace Graphics {

	class JarRenderPass {
		public:
			virtual ~JarRenderPass() = default;
	};


	class JarSurface {
		public:
			virtual ~JarSurface() = default;

			virtual void Update() = 0;

		//	virtual JarRenderPass *CreateRenderPass() = 0;
	};


	class JarBuffer {
		public:
			virtual ~JarBuffer() = default;
	};

	class JarShaderModule {
		public:
			virtual ~JarShaderModule() = default;

			virtual void Release() = 0;
	};

	class JarPipeline {
		public:
			virtual ~JarPipeline() = default;

			virtual void Release() = 0;
	};

	class JarCommandBuffer {
		public:
			virtual ~JarCommandBuffer() = default;

			virtual void StartRecording(JarRenderPass *renderPass) = 0;

			virtual void EndRecording() = 0;

			virtual void BindPipeline(std::shared_ptr<JarPipeline> pipeline) = 0;

			virtual void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) = 0;

			virtual void Draw() = 0;

			virtual void Present(std::shared_ptr<JarSurface> &surface) = 0;
	};


	class JarCommandQueue {
		public:
			virtual ~JarCommandQueue() = default;

			virtual JarCommandBuffer *getNextCommandBuffer() = 0;

			virtual void Release() = 0;
	};


	class JarDevice {
		public:
			virtual ~JarDevice() = default;

			virtual void Release() = 0;

			virtual std::shared_ptr<JarCommandQueue> CreateCommandQueue() = 0;

			virtual std::shared_ptr<JarBuffer> CreateBuffer(size_t bufferSize, const void *data) = 0;

			virtual std::shared_ptr<JarShaderModule> CreateShaderModule(std::string fileContent) = 0;

			virtual std::shared_ptr<JarPipeline> CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule,
			                                                    std::shared_ptr<JarShaderModule> fragmentModule) = 0;

	};


	class Backend {
		public:
			virtual ~Backend() = default;

			virtual std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider *windowHandleProvider) = 0;

		//	virtual std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface> &surface) = 0;
	};

}
#endif //JAREP_IRENDERER_HPP
