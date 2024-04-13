//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALAPI_HPP
#define JAREP_METALAPI_HPP

#if defined(__APPLE__)

#include "IRenderAPI.hpp"
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

	class MetalDevice;

	class MetalBuffer;

	class MetalImage;

	class MetalSurface final : public JarSurface {
		public:
			MetalSurface();

			~MetalSurface() override;

			bool CreateFromNativeWindowProvider(NativeWindowHandleProvider* windowHandleProvider);

			void RecreateSurface(uint32_t width, uint32_t height) override;

			void ReleaseSwapchain() override;

			uint32_t GetSwapchainImageAmount() override;

			JarExtent GetSurfaceExtent() override;

			void FinalizeSurface(std::shared_ptr<MetalDevice> device);

			[[nodiscard]] CA::MetalDrawable* getDrawable() const { return drawable; }

			[[nodiscard]] bool isSurfaceInitialized() const { return contentView != nullptr; }

			MTL::Texture* getDepthStencilTexture() const { return m_depthStencilTexture; }

			MTL::Texture* getMSAATexture() const { return m_msaaTexture; }

			MTL::Texture* acquireNewDrawTexture() {
				drawable = layer->nextDrawable();
				return drawable->texture();
			}

		private:
			std::shared_ptr<MetalDevice> metalDevice;
			NS::View* contentView;
			NS::Window* window;
			CGRect surfaceRect;
			CA::MetalLayer* layer;
			CA::MetalDrawable* drawable;
			MTL::Texture* m_msaaTexture;
			MTL::Texture* m_depthStencilTexture;
			uint32_t maxSwapchainImageCount;


			void createMsaaTexture();

			void createDepthStencilTexture();
	};

#pragma region MetalRenderPass{

	class MetalRenderPassBuilder : public JarRenderPassBuilder {
		public:
			MetalRenderPassBuilder();

			~MetalRenderPassBuilder() override;

			JarRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) override;

			JarRenderPassBuilder* AddDepthStencilAttachment(DepthAttachment depthStencilAttachment) override;

			std::shared_ptr<JarRenderPass>
			Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) override;

		private:
			MTL::RenderPassDescriptor* m_renderPassDescriptor;
			std::optional<ColorAttachment> m_colorAttachment;
			bool useDepthAttachment;

			static MTL::StoreAction storeActionToMetal(const StoreOp storeOp) {
				switch (storeOp) {
					case StoreOp::Store:
						return MTL::StoreActionStore;
					case StoreOp::DontCare:
						return MTL::StoreActionDontCare;
				}
				return {};
			}

			static MTL::LoadAction loadActionToMetal(const LoadOp loadOp) {
				switch (loadOp) {
					case LoadOp::Load:
						return MTL::LoadActionLoad;
					case LoadOp::Clear:
						return MTL::LoadActionClear;
					case LoadOp::DontCare:
						return MTL::LoadActionDontCare;
				}
				return {};
			}

			static MTL::ClearColor clearColorToMetal(ClearColor clearColor) {
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

			void UpdateRenderPassDescriptor(std::shared_ptr<MetalSurface> metalSurface);

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

			bool
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) override;

			void EndRecording() override;

			void BindPipeline(std::shared_ptr<JarPipeline> pipeline, uint32_t frameIndex) override;

			void BindDescriptors(std::vector<std::shared_ptr<JarDescriptor>> descriptors) override;

			void BindVertexBuffer(std::shared_ptr<JarBuffer> jarBuffer) override;

			void BindIndexBuffer(std::shared_ptr<JarBuffer> jarBuffer) override;

			void Draw() override;

			void DrawIndexed(size_t indexAmount) override;

			void Present(std::shared_ptr<JarSurface>& m_surface, std::shared_ptr<JarDevice> device) override;

			MTL::CommandBuffer* getCommandBuffer() const { return buffer; }

		private:
			MTL::CommandBuffer* buffer;
			MTL::RenderCommandEncoder* encoder;
			std::shared_ptr<MetalBuffer> indexBuffer;
			std::shared_ptr<MetalRenderPass> metalRenderPass;
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

			void Release() override;

			void Update(const void* data, size_t bufferSize) override;

			[[nodiscard]] std::optional<MTL::Buffer*> getBuffer();

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

#pragma region MetalDescriptorBinding{

	class MetalDescriptorBuilder final : public JarDescriptorBuilder {
		public:
			MetalDescriptorBuilder() = default;

			~MetalDescriptorBuilder() override;

			MetalDescriptorBuilder* SetBinding(uint32_t binding) override;

			MetalDescriptorBuilder* SetStageFlags(StageFlags stageFlags) override;

			std::shared_ptr<JarDescriptor> BuildUniformBufferDescriptor(std::shared_ptr<JarDevice> device,
			                                                            std::vector<std::shared_ptr<JarBuffer>> uniformBuffers) override;

			std::shared_ptr<JarDescriptor>
			BuildImageBufferDescriptor(std::shared_ptr<JarDevice> device, std::shared_ptr<JarImage> image) override;

		private:
			std::optional<uint32_t> m_binding;
			std::optional<StageFlags> m_stageFlags;
	};

	class MetalDescriptorLayout final : public JarDescriptorLayout {
		public:
			MetalDescriptorLayout() = default;

			~MetalDescriptorLayout() override;

			void Release() override;

	};

	class MetalDescriptorContent {
		public:
			MetalDescriptorContent() = default;

			virtual ~MetalDescriptorContent() = default;

			virtual void Release() = 0;

			virtual void BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding, StageFlags stageFlags) = 0;

	};

	class MetalDescriptor final : public JarDescriptor {
		public:
			MetalDescriptor(uint32_t binding, StageFlags stageFlags, std::shared_ptr<MetalDescriptorContent> content, std::shared_ptr<MetalDescriptorLayout> layout) :
					m_binding(binding), m_stageFlags(stageFlags), m_content(std::move(content)), m_descriptorLayout(std::move(layout)) {};

			~MetalDescriptor() override;

			void Release() override;

			std::shared_ptr<JarDescriptorLayout> GetDescriptorLayout() override;

			void BindContentToEncoder(MTL::RenderCommandEncoder* encoder);

		private:
			uint32_t m_binding;
			StageFlags m_stageFlags;
			std::shared_ptr<MetalDescriptorContent> m_content;
			std::shared_ptr<MetalDescriptorLayout> m_descriptorLayout;
	};

	class MetalUniformDescriptorContent : public MetalDescriptorContent {
		public:
			MetalUniformDescriptorContent(std::vector<std::shared_ptr<MetalBuffer>> buffers) : m_uniformBuffers(std::move(buffers)) {};

			~MetalUniformDescriptorContent() override;

			void Release() override;

			void BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding, StageFlags stageFlags) override;

		private:
			std::vector<std::shared_ptr<MetalBuffer>> m_uniformBuffers;
			uint8_t m_currentBufferIndex = 0;
	};

	class MetalImageDescriptorContent : public MetalDescriptorContent {
		public:
			MetalImageDescriptorContent(std::shared_ptr<MetalImage> image)
					: m_image(image) {};

			~MetalImageDescriptorContent() override;

			void Release() override;

			void BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding, StageFlags stageFlags) override;

		private:
			std::shared_ptr<MetalImage> m_image;
	};

#pragma endregion MetalDescriptorBinding }

#pragma region MetalPipeline{

	class MetalPipelineBuilder final : public JarPipelineBuilder {
		public:
			MetalPipelineBuilder() = default;

			~MetalPipelineBuilder() override;

			MetalPipelineBuilder* SetShaderStage(ShaderStage shaderStage) override;

			MetalPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			MetalPipelineBuilder* SetVertexInput(VertexInput vertexInput) override;

			MetalPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) override;

			MetalPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) override;

			MetalPipelineBuilder*
			BindDescriptorLayouts(std::vector<std::shared_ptr<JarDescriptorLayout>> descriptors) override;

			MetalPipelineBuilder* SetColorBlendAttachments(ColorBlendAttachment blendAttachment) override;

			MetalPipelineBuilder* SetDepthStencilState(DepthStencilState depthStencilState) override;

			std::shared_ptr<JarPipeline> Build(std::shared_ptr<JarDevice> device) override;

		private:
			MTL::Function* m_vertexShaderFunc;
			MTL::Function* m_fragmentShaderFunc;

			std::shared_ptr<JarRenderPass> m_renderPass;
			std::vector<MetalImageDescriptorContent> m_textureDescriptorBindings;

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
			              MTL::DepthStencilState* depthStencilState, std::shared_ptr<JarRenderPass> renderPass) :
					m_device(device), m_pipelineState(pipelineState),
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
			std::vector<MetalImageDescriptorContent> m_textureDescriptorBindings;
	};

#pragma endregion MetalPipeline }

#pragma region MetalBackend{

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

#pragma endregion MetalBackend }

#pragma region MetalImage{

	class MetalImageBuilder final : public JarImageBuilder {
		public:
			MetalImageBuilder() = default;

			~MetalImageBuilder() override;

			MetalImageBuilder* SetPixelFormat(PixelFormat format) override;

			MetalImageBuilder* SetImagePath(std::string path) override;

			MetalImageBuilder* EnableMipMaps(bool enabled) override;

			std::shared_ptr<JarImage> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<MTL::PixelFormat> m_pixelFormat;
			std::optional<std::string> m_imagePath;
			bool m_enableMipMapping;

			static void generateMipMaps(std::shared_ptr<MetalDevice>& device, MTL::Texture* texture);
	};

	class MetalImage final : public JarImage {
		public:
			MetalImage(MTL::Texture* texture, MTL::SamplerState* sampler) : m_texture(texture),
			                                                                m_samplerState(sampler) {}

			~MetalImage() override;

			void Release() override;

			[[nodiscard]] MTL::Texture* getTexture() const { return m_texture; }

			[[nodiscard]] MTL::SamplerState* getSampler() const { return m_samplerState; }

		private:
			MTL::Texture* m_texture;
			MTL::SamplerState* m_samplerState;
	};

#pragma endregion MetalImage }
}
#endif
#endif //JAREP_METALAPI_HPP
