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

			void FinalizeSurface(MTL::Device *device);

			[[nodiscard]] CA::MetalDrawable *getDrawable() const { return drawable; }

			[[nodiscard]] bool isSurfaceInitialized() const { return contentView != nullptr; }

			MTL::Texture *acquireNewDrawTexture() {
				drawable = layer->nextDrawable();
				return drawable->texture();
			}

		private:
			NS::View *contentView;
			NS::Window *window;
			CGRect surfaceRect;
			CA::MetalLayer *layer;
			CA::MetalDrawable *drawable;

	};

#pragma region MetalRenderPass{

	class MetalRenderPassBuilder : public JarRenderPassBuilder {
		public:
			MetalRenderPassBuilder();

			~MetalRenderPassBuilder() override;

			JarRenderPassBuilder *AddColorAttachment(Graphics::ColorAttachment colorAttachment) override;

			std::shared_ptr<JarRenderPass> Build(std::shared_ptr<JarDevice> device) override;

		private:
			MTL::RenderPassDescriptor *m_renderPassDescriptor;
			std::optional<ColorAttachment> m_colorAttachment;

			static MTL::StoreAction storeActionToMetal(StoreOp storeOp) {
				switch (storeOp) {
					case StoreOp::Store:
						return MTL::StoreActionStore;
					case StoreOp::DontCare:
						return MTL::StoreActionDontCare;
				}
			}

			static MTL::LoadAction loadActionToMetal(LoadOp loadOp) {
				switch (loadOp) {

					case LoadOp::Load:
						return MTL::LoadActionLoad;
					case LoadOp::Clear:
						return MTL::LoadActionClear;
					case LoadOp::DontCare:
						return MTL::LoadActionDontCare;
				}
			}

			static MTL::ClearColor clearColorToMetal(ClearColor clearColor) {
				return MTL::ClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			}

	};

	class MetalRenderPass final : public JarRenderPass {
		public:
			explicit MetalRenderPass(MTL::RenderPassDescriptor *rpd) : renderPassDesc(rpd) {
			}

			~MetalRenderPass() override;

			void Release() override;

			[[nodiscard]] MTL::RenderPassDescriptor *getRenderPassDesc() const { return renderPassDesc; }

		private:
			MTL::RenderPassDescriptor *renderPassDesc;
	};

#pragma endregion MetalRenderPass }

	class MetalCommandBuffer final : public JarCommandBuffer {
		public:
			explicit MetalCommandBuffer(MTL::CommandBuffer *cmdBuffer) : buffer(cmdBuffer) {
				encoder = nullptr;
			}

			~MetalCommandBuffer() override;

			void
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) override;

			void EndRecording() override;

			void BindPipeline(std::shared_ptr<JarPipeline> pipeline) override;

			void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) override;

			void Draw() override;

			void Present(std::shared_ptr<JarSurface> &m_surface, std::shared_ptr<JarDevice> device) override;

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

			void Release(std::shared_ptr<JarDevice> device) override;

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

			std::shared_ptr<JarBuffer> CreateBuffer(size_t bufferSize, const void *data) override;

			std::shared_ptr<JarShaderModule> CreateShaderModule(std::string shaderContent) override;

			std::shared_ptr<JarPipeline> CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule,
			                                            std::shared_ptr<JarShaderModule> fragmentModule,
			                                            std::shared_ptr<JarRenderPass> renderPass) override;

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

	class MetalShaderLibrary final : public JarShaderModule {
		public:
			MetalShaderLibrary() = default;

			~MetalShaderLibrary() override;

			void CreateShaderLibrary(MTL::Device *device, std::string shaderContent);

			void Release(std::shared_ptr<JarDevice> device) override;

			MTL::Library *getLibrary() { return library; }

		private:
			MTL::Library *library;
	};

	class MetalPipeline final : public JarPipeline {
		public:
			MetalPipeline() = default;

			~MetalPipeline() override;

			std::shared_ptr<JarRenderPass> GetRenderPass() override;

			void CreatePipeline(MTL::Device *device, MTL::Library *vertexLib, MTL::Library *fragmentLib);

			void Release() override;

			MTL::RenderPipelineState *getPSO() { return pipelineState; }

		private:
			MTL::RenderPipelineState *pipelineState;
	};

	class MetalBackend final : public Backend {
		public:
			MetalBackend();

			~MetalBackend() override;

			std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider *windowHandleProvider) override;

			std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface> &m_surface) override;

			JarRenderPassBuilder* InitRenderPassBuilder() override;
	};
}
#endif
#endif //JAREP_METALAPI_HPP
