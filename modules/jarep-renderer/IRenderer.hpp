//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

#include "NativeWindowHandleProvider.hpp"

namespace Graphics {
	class JarRenderPass {
		public:
			virtual ~JarRenderPass() = default;
	};

	class JarSurface {
		public:
			virtual ~JarSurface() = default;

			virtual void Update() = 0;

			virtual JarRenderPass *CreateRenderPass() = 0;
	};


	class JarCommandBuffer {
		public:
			virtual ~JarCommandBuffer() = default;

			virtual void StartRecording(JarRenderPass *renderPass) = 0;

			virtual void EndRecording() = 0;

			virtual void Present(std::shared_ptr<JarSurface> &surface) = 0;
	};


	class JarCommandQueue {
		public:
			virtual ~JarCommandQueue() = default;

			virtual JarCommandBuffer *getNextCommandBuffer() = 0;

			virtual void Release() = 0;
	};

	class JarBuffer {
		public:
			virtual ~JarBuffer() = default;
	};

	class JarDevice {
		public:
			virtual ~JarDevice() = default;

			virtual void Release() = 0;

			virtual std::shared_ptr<JarCommandQueue> CreateCommandQueue() = 0;

			virtual JarBuffer *CreateBuffer(size_t bufferSize, const void *data) = 0;
	};


	class Backend {
		public:
			virtual ~Backend() = default;

			virtual std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider *windowHandleProvider) = 0;

			virtual std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface> &surface) = 0;
	};

	class IRenderer {
		public:
			virtual ~IRenderer() = default;

			/// Get the physical device (GPU) for render processes.
			virtual void RegisterPhysicalDevice() = 0;

			/// Create a logical device to be able to perform operations on the GPU
			virtual void CreateLogicalDevice() = 0;

			/// Create a rendering surface for the given window handle
			virtual void CreateSurface(NativeWindowHandleProvider *windowHandleProvider) = 0;

			virtual void CreateVertexBuffer() = 0;

			virtual void CreateShaders() = 0;

			virtual void CreateCommandQueue() = 0;

			virtual void CreateGraphicsPipeline() = 0;

			virtual void RecordCommandBuffer() = 0;

			virtual void Draw() = 0;

			virtual void Shutdown() = 0;
	};
}
#endif //JAREP_IRENDERER_HPP
