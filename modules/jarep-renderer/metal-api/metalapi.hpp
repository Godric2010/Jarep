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
#include <utility>

namespace Graphics::Metal {
	class MetalSurface final : public JarSurface {
		public:
			MetalSurface();

			~MetalSurface() override;

			bool CreateFromNativeWindowProvider(NativeWindowHandleProvider* windowHandleProvider);

			void Update() override;

			void FinalizeSurface(MTL::Device* device);

			[[nodiscard]] CA::MetalDrawable* getDrawable() const { return drawable; }

			[[nodiscard]] bool isSurfaceInitialized() const { return contentView != nullptr; }

			MTL::Texture* acquireNewDrawTexture() {
				drawable = layer->nextDrawable();
				return drawable->texture();
			}

		private:
			NS::View* contentView;
			NS::Window* window;
			CGRect surfaceRect;
			CA::MetalLayer* layer;
			CA::MetalDrawable* drawable;
	};

#pragma region MetalRenderPass{

	class MetalRenderPassBuilder : public JarRenderPassBuilder {
		public:
			MetalRenderPassBuilder();

			~MetalRenderPassBuilder() override;

			JarRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) override;

			std::shared_ptr<JarRenderPass> Build(std::shared_ptr<JarDevice> device) override;

		private:
			MTL::RenderPassDescriptor* m_renderPassDescriptor;
			std::optional<ColorAttachment> m_colorAttachment;

			static MTL::StoreAction storeActionToMetal(const StoreOperation storeOp) {
				switch (storeOp) {
					case StoreOperation::Store:
						return MTL::StoreActionStore;
					case StoreOperation::DontCare:
						return MTL::StoreActionDontCare;
				}
				return {};
			}

			static MTL::LoadAction loadActionToMetal(const LoadOperation loadOp) {
				switch (loadOp) {
					case LoadOperation::Load:
						return MTL::LoadActionLoad;
					case LoadOperation::Clear:
						return MTL::LoadActionClear;
					case LoadOperation::DontCare:
						return MTL::LoadActionDontCare;
				}
				return {};
			}

			static MTL::Clear clearColorToMetal(Clear clearColor) {
				return {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
			}
	};

	class MetalRenderPass final : public JarRenderPass {
		public:
			explicit MetalRenderPass(MTL::RenderPassDescriptor* rpd) : renderPassDesc(rpd) {
			}

			~MetalRenderPass() override;

			void Release() override;

			[[nodiscard]] MTL::RenderPassDescriptor* getRenderPassDesc() const { return renderPassDesc; }

		private:
			MTL::RenderPassDescriptor* renderPassDesc;
	};

#pragma endregion MetalRenderPass }

	class MetalCommandBuffer final : public JarCommandBuffer {
		public:
			explicit MetalCommandBuffer(MTL::CommandBuffer* cmdBuffer) : buffer(cmdBuffer) {
				encoder = nullptr;
			}

			~MetalCommandBuffer() override;

			void
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) override;

			void EndRecording() override;

			void BindPipeline(std::shared_ptr<JarPipeline> pipeline) override;

			void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) override;

			void Draw() override;

			void Present(std::shared_ptr<JarSurface>& m_surface, std::shared_ptr<JarDevice> device) override;

		private:
			MTL::CommandBuffer* buffer;
			MTL::RenderCommandEncoder* encoder;
	};

#pragma region CommandQueue{

	class MetalCommandQueueBuilder final : public JarCommandQueueBuilder {
		public:
			MetalCommandQueueBuilder() = default;

			~MetalCommandQueueBuilder() override;

			MetalCommandQueueBuilder* SetCommandBufferAmount(uint32_t commandBufferAmount) override;

			std::shared_ptr<JarCommandQueue> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<uint32_t> m_amountOfCommandBuffers;
			const uint32_t DEFAULT_COMMAND_BUFFER_COUNT = 3;
	};

	class MetalCommandQueue final : public JarCommandQueue {
		public:
			MetalCommandQueue(MTL::CommandQueue* cmdQueue) : queue(cmdQueue) {
			}

			~MetalCommandQueue() override;

			JarCommandBuffer* getNextCommandBuffer() override;

			void Release() override;

		private:
			MTL::CommandQueue* queue;
	};

#pragma endregion CommandQueue }

	class MetalDevice final : public JarDevice {
		public:
			MetalDevice() { _device = std::nullopt; }

			~MetalDevice() override;

			void Initialize();

			void Release() override;

			[[nodiscard]] std::optional<MTL::Device*> getDevice() const;

		private:
			std::optional<MTL::Device*> _device;
	};

#pragma region MetalBuffer{

	class MetalBufferBuilder final : public JarBufferBuilder {
		public:
			MetalBufferBuilder() = default;

			~MetalBufferBuilder() override;

			MetalBufferBuilder* SetUsageFlags(BufferUsage usageFlags) override;

			MetalBufferBuilder* SetMemoryProperties(MemoryProperties memProps) override;

			MetalBufferBuilder* SetBufferData(const void* data, size_t bufferSize) override;

			std::shared_ptr<JarBuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<BufferUsage> m_bufferUsage;
			std::optional<MemoryProperties> m_memoryProperties;
			std::optional<const void*> m_data;
			size_t m_bufferSize;

			static MTL::ResourceUsage bufferUsageToMetal(BufferUsage usage);

			static MTL::ResourceOptions memoryPropertiesToMetal(MemoryProperties memProps);
	};

	class MetalBuffer final : public JarBuffer {
		public:
			explicit MetalBuffer(MTL::Buffer* buffer) : m_buffer(buffer) {
			}

			~MetalBuffer() override;

			std::optional<MTL::Buffer*> getBuffer();

		private:
			MTL::Buffer* m_buffer;
	};

#pragma endregion MetalBuffer }
#pragma region MetalShaderLibrary{

	class MetalShaderLibraryBuilder final : public JarShaderModuleBuilder {
		public:
			MetalShaderLibraryBuilder() = default;

			~MetalShaderLibraryBuilder() override;

			MetalShaderLibraryBuilder* SetShader(std::string shaderCode) override;

			MetalShaderLibraryBuilder* SetShaderType(ShaderType shaderType) override;

			std::shared_ptr<JarShaderModule> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<NS::String*> m_shaderCodeString;
			std::optional<ShaderType> m_shaderTypeOpt;
	};

	class MetalShaderLibrary final : public JarShaderModule {
		public:
			explicit MetalShaderLibrary(MTL::Library* library) : m_library(library) {
			}

			~MetalShaderLibrary() override;

			void Release() override;

			[[nodiscard]] MTL::Library* getLibrary() const { return m_library; }

		private:
			MTL::Library* m_library;
	};

#pragma endregion MetalShaderLibrary }

	class MetalPipelineBuilder final : public JarPipelineBuilder {
		public:
			MetalPipelineBuilder() = default;

			~MetalPipelineBuilder() override;

			MetalPipelineBuilder* SetShaderStage(ShaderStage shaderStage) override;

			MetalPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			MetalPipelineBuilder* SetVertexInput(VertexInput vertexInput) override;

			MetalPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) override;

			MetalPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) override;

			MetalPipelineBuilder* SetColorBlendAttachments(ColorBlendAttachment blendAttachment) override;

			MetalPipelineBuilder* SetDepthStencilState(DepthStencilState depthStencilState) override;

			std::shared_ptr<JarPipeline> Build(std::shared_ptr<JarDevice> device) override;

		private:
			MTL::Function* m_vertexShaderFunc;
			MTL::Function* m_fragmentShaderFunc;

			std::shared_ptr<JarRenderPass> m_renderPass;

			MTL::VertexDescriptor* m_vertexDescriptor;
			MTL::PrimitiveTopologyClass m_topology;

			uint16_t m_multisamplingCount;

			std::vector<MTL::RenderPipelineColorAttachmentDescriptor*> m_colorAttachments;
			MTL::DepthStencilDescriptor* m_depthStencilDescriptor;
			MTL::StencilDescriptor* m_stencilDescriptor;

			MTL::ColorWriteMask convertToMetalColorWriteMask(ColorWriteMask mask);
	};

	class MetalPipeline final : public JarPipeline {
		public:
			MetalPipeline(MTL::Device* device, MTL::RenderPipelineState* pipelineState,
			              MTL::DepthStencilState* depthStencilState, std::shared_ptr<JarRenderPass> renderPass)
					: m_device(device), m_pipelineState(pipelineState),
					  m_depthStencilState(depthStencilState), m_renderPass(std::move(renderPass)) {};

			~MetalPipeline() override;

			void Release() override;

			std::shared_ptr<JarRenderPass> GetRenderPass() override { return m_renderPass; }

			MTL::RenderPipelineState* getPSO() { return m_pipelineState; }

			MTL::DepthStencilState* getDSS() { return m_depthStencilState; }

		private:
			MTL::RenderPipelineState* m_pipelineState;
			MTL::DepthStencilState* m_depthStencilState;
			MTL::Device* m_device;
			std::shared_ptr<JarRenderPass> m_renderPass;
	};

	class MetalBackend final : public Backend {
		public:
			MetalBackend();

			~MetalBackend() override;

			std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* windowHandleProvider) override;

			std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& m_surface) override;

			JarShaderModuleBuilder* InitShaderModuleBuilder() override;

			JarRenderPassBuilder* InitRenderPassBuilder() override;

			JarCommandQueueBuilder* InitCommandQueueBuilder() override;

			JarBufferBuilder* InitBufferBuilder() override;

			JarPipelineBuilder* InitPipelineBuilder() override;
	};
}
#endif
#endif //JAREP_METALAPI_HPP
