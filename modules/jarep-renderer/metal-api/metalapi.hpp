//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALAPI_HPP
#define JAREP_METALAPI_HPP

#if defined(__APPLE__)

#include "IRenderer.hpp"
#include "sdlsurfaceadapter.hpp"
#include <Metal/Metal.hpp>
#include <Foundation/Foundation.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>
#include <functional>
#include <fstream>

namespace Graphics::Metal {

	class MetalSurface final : public JarSurface {
		public:
			MetalSurface();

			~MetalSurface() override;

			bool CreateFromNativeWindowProvider(NativeWindowHandleProvider *windowHandleProvider);

			void Update() override;

			JarRenderPass *CreateRenderPass() override;

			void FinalizeSurface(MTL::Device *device);

			[[nodiscard]] CA::MetalDrawable *getDrawable() const { return drawable; }

		private:
			MTK::View *view;
			NS::View *contentView;
			NS::Window *window;
			CGRect surfaceRect;
			CA::MetalLayer *layer;
			CA::MetalDrawable *drawable;

	};

	class MetalRenderPass final : public JarRenderPass {
		public:
			explicit MetalRenderPass(MTL::RenderPassDescriptor *rpd) : renderPassDesc(rpd) {
			}

			~MetalRenderPass() override;

			[[nodiscard]] MTL::RenderPassDescriptor *getRenderPassDesc() const { return renderPassDesc; }

		private:
			MTL::RenderPassDescriptor *renderPassDesc;
	};

	class MetalCommandBuffer final : public JarCommandBuffer {
		public:
			explicit MetalCommandBuffer(MTL::CommandBuffer *cmdBuffer) : buffer(cmdBuffer) {
				encoder = nullptr;
			}

			~MetalCommandBuffer() override;

			void StartRecording(JarRenderPass *renderPass) override;

			void EndRecording() override;

			void Present(std::shared_ptr<JarSurface> &surface) override;

		private:
			MTL::CommandBuffer *buffer;
			MTL::RenderCommandEncoder *encoder;
	};

	class MetalCommandQueue final : public JarCommandQueue {
		public:
			MetalCommandQueue(MTL::CommandQueue *cmdQueue) : queue(cmdQueue) {
			}

			~MetalCommandQueue() override;

			JarCommandBuffer *getNextCommandBuffer() override;

			void Release() override;

		private:
			MTL::CommandQueue *queue;
	};

	class MetalDevice final : public JarDevice {
		public:
			MetalDevice() { _device = std::nullopt; }

			~MetalDevice() override;

			void Initialize();

			void Release() override;

			std::shared_ptr<JarCommandQueue> CreateCommandQueue() override;

			JarBuffer *CreateBuffer(size_t bufferSize, const void *data) override;

			JarShaderModule* CreateShaderModule(std::string shaderContent) override;

			[[nodiscard]] std::optional<MTL::Device *> getDevice() const;

		private:
			std::optional<MTL::Device *> _device;
	};

	class MetalBuffer final : public JarBuffer {
		public:
			MetalBuffer() = default;

			~MetalBuffer() override;

			void CreateBuffer(size_t size, const void *data, MTL::Device *metalDevice);

			std::optional<MTL::Buffer *> getBuffer();

		private:
			MTL::Buffer *buffer;
	};

	class MetalShaderLibrary final: public JarShaderModule{
		public:
			MetalShaderLibrary() = default;
			~MetalShaderLibrary() override;

			void CreateShaderLibrary(MTL::Device* device, std::string shaderContent);

			void Release() override;

		private:
			MTL::Library* library;
	};

	class MetalBackend final : public Backend {
		public:
			MetalBackend();

			~MetalBackend() override;

			std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider *windowHandleProvider) override;

			std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface> &surface) override;
	};

	class MetalAPI : public IRenderer {
		public:
			MetalAPI();

			~MetalAPI() override;

			/// Create a logical device for metal means to link the MTLdevice with the surface and push this into the window settings.
			void CreateLogicalDevice() override;

			/// Create an MTLDevice and provide it for future usage
			void RegisterPhysicalDevice() override;

			/// Create a render surface but don't push it to the window, since the creation of the MTL Device occurs later
			void CreateSurface(NativeWindowHandleProvider *nativeWindowHandle) override;

			void CreateVertexBuffer() override;

			void CreateShaders() override;

			void CreateCommandQueue() override;

			void CreateGraphicsPipeline() override;

			void RecordCommandBuffer() override;

			void Draw() override;

			void Shutdown() override;

		private:
			NS::Window *window;
			MTK::View *surface;
			CA::MetalLayer *metalLayer;
			MTL::Device *device;
			//			CA::MetalDrawable* metalDrawable;

			MTL::Library *vertShaderLibrary;
			MTL::Library *fragShaderLibrary;
			MTL::CommandQueue *commandQueue;
			MTL::CommandBuffer *commandBuffer;
			MTL::RenderPipelineState *renderPSO;
			MTL::Buffer *triangleVertexBuffer;
			//			MTL::RenderPassDescriptor* renderPassDescriptor;

			std::string readFile(const std::string &filename);

			void encodeRenderCommand(MTL::RenderCommandEncoder *renderCommandEncoder);
	};
}
#endif
#endif //JAREP_METALAPI_HPP
