//
// Created by Sebastian Borsch on 24.10.23.
//
#include <iostream>
#include <utility>

#if defined(__APPLE__)

#include "metalapi.hpp"

namespace Graphics::Metal {

	static std::unordered_map<PixelFormat, MTL::PixelFormat> pixelFormatMap{
			{PixelFormat::R8Unorm,              MTL::PixelFormatR8Unorm},
			{PixelFormat::RG8Unorm,             MTL::PixelFormatRG8Unorm},
			{PixelFormat::RGBA8Unorm,           MTL::PixelFormatRGBA8Unorm},
			{PixelFormat::BGRA8Unorm,           MTL::PixelFormatBGRA8Unorm},
			{PixelFormat::R16Unorm,             MTL::PixelFormatR16Unorm},
			{PixelFormat::RG16Unorm,            MTL::PixelFormatRG16Unorm},
			{PixelFormat::RGBA16Unorm,          MTL::PixelFormatRGBA16Unorm},
			{PixelFormat::RGBA16Float,          MTL::PixelFormatRGBA16Float},
			{PixelFormat::R32Float,             MTL::PixelFormatR32Float},
			{PixelFormat::RG32Float,            MTL::PixelFormatRG32Float},
			{PixelFormat::RGBA32Float,          MTL::PixelFormatRGBA32Float},
			{PixelFormat::Depth32Float,         MTL::PixelFormatDepth32Float},
			{PixelFormat::Depth24Stencil8,      MTL::PixelFormatDepth24Unorm_Stencil8},
			{PixelFormat::Depth32FloatStencil8, MTL::PixelFormatDepth32Float_Stencil8},
			{PixelFormat::Depth16Unorm,         MTL::PixelFormatDepth16Unorm}
	};

#pragma region MetalBackend{

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


#pragma endregion MetalBackend }

#pragma region MetalSurface{

	MetalSurface::MetalSurface() = default;

	MetalSurface::~MetalSurface() = default;


	bool MetalSurface::CreateFromNativeWindowProvider(NativeWindowHandleProvider* windowHandleProvider) {
		window = static_cast<NS::Window*>(windowHandleProvider->getNativeWindowHandle());


		surfaceRect = CGRectMake(0, 0, windowHandleProvider->getWindowWidth(),
		                         windowHandleProvider->getWindowHeight());
		return true;
	}

	void MetalSurface::RecreateSurface(uint32_t width, uint32_t height) {
		surfaceRect = CGRectMake(0, 0, width, height);
		layer->setDrawableSize(surfaceRect.size);
	}

	void MetalSurface::ReleaseSwapchain() {
	}

	uint32_t MetalSurface::GetSwapchainImageAmount() {
		return maxSwapchainImageCount;
	}

	JarExtent MetalSurface::GetSurfaceExtent() {
		return JarExtent{static_cast<float>(surfaceRect.size.width), static_cast<float>(surfaceRect.size.height)};
	}

	void MetalSurface::FinalizeSurface(std::shared_ptr<MetalDevice> device) {
		Graphics::Metal::SDLSurfaceAdapter::CreateViewAndMetalLayer(surfaceRect, &contentView, &layer,
		                                                            &maxSwapchainImageCount);

		metalDevice = device;
		if (contentView == nullptr)
			throw std::runtime_error("Expected NS::View* to be not nullptr!");

		if (layer == nullptr)
			throw std::runtime_error("Expected metal layer not to be nullptr");

		layer->setDevice(metalDevice->getDevice().value());
		layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		layer->setDrawableSize(surfaceRect.size);

		window->setContentView(contentView);
		SDLSurfaceAdapter::getDrawableFromMetalLayer(layer, &drawable);
	}

	MTL::PixelFormat MetalSurface::getDrawablePixelFormat() const {
		return drawable->texture()->pixelFormat();
	}

#pragma endregion MetalSurface }

#pragma region MetalDevice {

	MetalDevice::~MetalDevice() = default;

	void MetalDevice::Initialize() {
		_device = std::make_optional(MTL::CreateSystemDefaultDevice());
	}

	std::optional<MTL::Device*> MetalDevice::getDevice() const {
		return _device;
	}

	void MetalDevice::Release() {
		if (!_device.has_value()) return;
		_device.value()->release();
	}

	uint32_t MetalDevice::GetMaxUsableSampleCount() {
		MTL::Device* device = _device.value();
		std::vector<uint32_t> possibleSampleCounts = {64, 32, 16, 8, 4, 2};
		for (const auto sampleCount: possibleSampleCounts) {
			if (device->supportsTextureSampleCount(sampleCount))
				return sampleCount;
		}
		return 1;
	}

	bool MetalDevice::IsFormatSupported(Graphics::PixelFormat format) {
		MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::alloc()->init();
		descriptor->setTextureType(MTL::TextureType2D);
		descriptor->setPixelFormat(pixelFormatMap[format]);
		descriptor->setWidth(1);  // Minimum size
		descriptor->setHeight(1);
		descriptor->setDepth(1);
		descriptor->setStorageMode(MTL::StorageModePrivate);
		descriptor->setUsage(MTL::TextureUsageRenderTarget);

		MTL::Texture* testTexture = _device.value()->newTexture(descriptor);
		bool isSupported = (testTexture != nullptr);
		if (testTexture) {
			testTexture->release();  // Clean up
		}
		descriptor->release();
		return isSupported;
	}

#pragma endregion MetalDevice }

#pragma region MetalCommandQueue {

	MetalCommandQueueBuilder::~MetalCommandQueueBuilder() = default;

	MetalCommandQueueBuilder* MetalCommandQueueBuilder::SetCommandBufferAmount(uint32_t commandBufferAmount) {
		m_amountOfCommandBuffers = std::make_optional(commandBufferAmount);
		return this;
	}

	std::shared_ptr<JarCommandQueue> MetalCommandQueueBuilder::Build(std::shared_ptr<JarDevice> device) {
		const auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		uint32_t commandBuffersCount;
		if (m_amountOfCommandBuffers.has_value())
			commandBuffersCount = m_amountOfCommandBuffers.value();
		else
			commandBuffersCount = DEFAULT_COMMAND_BUFFER_COUNT;

		const auto amountOfCommandBuffers = static_cast<NS::UInteger>(commandBuffersCount);
		auto commandQueue = metalDevice->getDevice().value()->newCommandQueue(amountOfCommandBuffers);
		return std::make_shared<MetalCommandQueue>(commandQueue);
	}

	MetalCommandQueue::~MetalCommandQueue() = default;

	JarCommandBuffer* MetalCommandQueue::getNextCommandBuffer() {
		return new MetalCommandBuffer(queue->commandBuffer());
	}

	void MetalCommandQueue::Release() {
		queue->release();
	}

#pragma endregion MetalComandQueue }

#pragma region MetalCommandBuffer {

	MetalCommandBuffer::~MetalCommandBuffer() = default;

	bool
	MetalCommandBuffer::StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) {
		metalRenderPass = reinterpret_cast<std::shared_ptr<MetalRenderPass>&>(renderPass);
		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);

		auto renderPassDesc = metalRenderPass->getRenderPassDesc();
		renderPassDesc->colorAttachments()->object(0)->setTexture(metalSurface->acquireNewDrawTexture());
		encoder = buffer->renderCommandEncoder(renderPassDesc);
		return true;
	}

	void MetalCommandBuffer::EndRecording() {
		encoder->endEncoding();
	}

	void MetalCommandBuffer::BindPipeline(std::shared_ptr<Graphics::JarPipeline> pipeline, uint32_t frameIndex) {
		auto metalPipeline = reinterpret_cast<MetalPipeline*>(pipeline.get());
		encoder->setRenderPipelineState(metalPipeline->getPSO());

		auto depthStencilState = metalPipeline->getDSS();
		if (depthStencilState.has_value())
			encoder->setDepthStencilState(depthStencilState.value());

		encoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);
		encoder->setCullMode(MTL::CullMode::CullModeBack);
	}

	void MetalCommandBuffer::BindDescriptors(std::vector<std::shared_ptr<JarDescriptor>> descriptors) {
		auto metalDescriptors = reinterpret_cast<std::vector<std::shared_ptr<MetalDescriptor>>&>(descriptors);
		for (auto& descriptor: metalDescriptors) {
			descriptor->BindContentToEncoder(encoder);
		}
	}

	void MetalCommandBuffer::BindVertexBuffer(std::shared_ptr<Graphics::JarBuffer> jarBuffer) {
		auto* metalBuffer = reinterpret_cast<MetalBuffer*>(jarBuffer.get());
		encoder->setVertexBuffer(metalBuffer->getBuffer().value(), 0, 0);
	}

	void MetalCommandBuffer::BindIndexBuffer(std::shared_ptr<JarBuffer> jarBuffer) {
		std::shared_ptr<MetalBuffer> metalBuffer = reinterpret_cast<std::shared_ptr<MetalBuffer>&>(jarBuffer);
		indexBuffer = metalBuffer;
	}

	void MetalCommandBuffer::SetDepthBias(Graphics::DepthBias depthBias) {
		encoder->setDepthBias(depthBias.DepthBiasConstantFactor, depthBias.DepthBiasClamp,
		                      depthBias.DepthBiasSlopeFactor);
	}

	void MetalCommandBuffer::SetViewport(Graphics::Viewport viewport) {
		auto metalViewport = MTL::Viewport();
		metalViewport.height = viewport.height;
		metalViewport.width = viewport.width;
		metalViewport.originX = viewport.x;
		metalViewport.originY = viewport.y;
		metalViewport.znear = viewport.minDepth;
		metalViewport.zfar = viewport.maxDepth;
		encoder->setViewport(metalViewport);
	}

	void MetalCommandBuffer::SetScissor(Graphics::Scissor scissor) {
		auto metalScissor = MTL::ScissorRect();
		metalScissor.x = scissor.x;
		metalScissor.y = scissor.y;
		metalScissor.width = scissor.width;
		metalScissor.height = scissor.height;
		encoder->setScissorRect(metalScissor);
	}

	void MetalCommandBuffer::DrawIndexed(size_t indexAmount) {
		encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(indexAmount), MTL::IndexTypeUInt16,
		                               indexBuffer->getBuffer().value(), 0);
	}

	void MetalCommandBuffer::Draw() {
		encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
	}

	void MetalCommandBuffer::Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) {
		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);

		metalRenderPass->UpdateRenderPassDescriptor(metalSurface);

		buffer->presentDrawable(metalSurface->getDrawable());
		buffer->commit();
		buffer->waitUntilCompleted();
	}


#pragma endregion MetalCommandBuffer }

#pragma region MetalRenderPass{

	MetalRenderPassBuilder::MetalRenderPassBuilder() {
		m_renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
		m_colorAttachment = std::nullopt;
	}

	MetalRenderPassBuilder::~MetalRenderPassBuilder() =
	default;

	JarRenderPassBuilder* MetalRenderPassBuilder::AddColorAttachment(Graphics::ColorAttachment colorAttachment) {
		m_colorAttachment = std::make_optional(colorAttachment);

		MTL::RenderPassColorAttachmentDescriptor* cd = m_renderPassDescriptor->colorAttachments()->object(0);
		cd->setLoadAction(loadActionToMetal(colorAttachment.loadOp));
		cd->setClearColor(clearColorToMetal(colorAttachment.clearColor));
		cd->setStoreAction(storeActionToMetal(colorAttachment.storeOp));
		return this;
	}

	JarRenderPassBuilder* MetalRenderPassBuilder::AddDepthStencilAttachment(
			Graphics::DepthAttachment depthStencilAttachment) {

		MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = m_renderPassDescriptor->depthAttachment();
		depthAttachment->setLoadAction(loadActionToMetal(depthStencilAttachment.DepthLoadOp));
		depthAttachment->setClearDepth(depthStencilAttachment.DepthClearValue);
		depthAttachment->setStoreAction(storeActionToMetal(depthStencilAttachment.DepthStoreOp));

		m_depthStencilFormat = std::make_optional(pixelFormatMap[depthStencilAttachment.Format]);
		return this;
	}

	JarRenderPassBuilder* MetalRenderPassBuilder::SetMultisamplingCount(uint8_t multisamplingCount) {
		m_multisamplingCount = std::make_optional(multisamplingCount);
		return this;
	}

	std::shared_ptr<JarRenderPass>
	MetalRenderPassBuilder::Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) {
		if (!m_colorAttachment.has_value())
			throw std::exception();

		auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		auto renderPassImages = new MetalRenderPassImages(metalDevice);

		renderPassImages->CreateRenderPassImages(metalSurface->GetSurfaceExtent().Width,
		                                         metalSurface->GetSurfaceExtent().Height,
		                                         m_multisamplingCount.value(),
		                                         metalSurface->getDrawablePixelFormat(),
		                                         m_depthStencilFormat);

		auto colorAttachmentDesc = m_renderPassDescriptor->colorAttachments()->object(0);
		colorAttachmentDesc->setTexture(renderPassImages->getMSAATexture());
		colorAttachmentDesc->setResolveTexture(metalSurface->getDrawable()->texture());
		colorAttachmentDesc->setStoreAction(MTL::StoreActionMultisampleResolve);

		if (m_depthStencilFormat.has_value())
			m_renderPassDescriptor->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
		return std::make_shared<MetalRenderPass>(m_renderPassDescriptor, renderPassImages);
	}

	void MetalRenderPassImages::CreateRenderPassImages(uint32_t width, uint32_t height, uint8_t multisamplingCount,
	                                                   MTL::PixelFormat colorFormat,
	                                                   std::optional<MTL::PixelFormat> depthStencilFormat) {
		m_multisamplingCount = multisamplingCount;
		createMSAATexture(width, height, colorFormat, multisamplingCount);
		if (depthStencilFormat.has_value()) {
			m_depthStencilFormat = depthStencilFormat.value();
			createDepthStencilTexture(width, height, depthStencilFormat.value(), m_multisamplingCount);
		}

	}

	void MetalRenderPassImages::RecreateRenderPassImages(uint32_t width, uint32_t height, MTL::PixelFormat msaaFormat) {
		m_msaaTexture->release();
		m_depthStencilTexture->release();
		createMSAATexture(width, height, msaaFormat, m_multisamplingCount);
		if (m_depthStencilTexture != nullptr)
			createDepthStencilTexture(width, height, m_depthStencilFormat, m_multisamplingCount);
	}

	void MetalRenderPassImages::Release() {
		m_msaaTexture->release();
		if (m_depthStencilTexture != nullptr)
			m_depthStencilTexture->release();
	}

	void MetalRenderPassImages::createMSAATexture(uint32_t width, uint32_t height, MTL::PixelFormat format,
	                                              uint8_t multisamplingCount) {
		MTL::TextureDescriptor* msaaDesc = MTL::TextureDescriptor::alloc()->init();
		msaaDesc->setTextureType(MTL::TextureType2DMultisample);
		msaaDesc->setPixelFormat(format);
		msaaDesc->setWidth(width);
		msaaDesc->setHeight(height);
		msaaDesc->setSampleCount(multisamplingCount);
		msaaDesc->setUsage(MTL::TextureUsageRenderTarget);
		m_msaaTexture = m_device->getDevice().value()->newTexture(msaaDesc);

		msaaDesc->release();
	}

	void MetalRenderPassImages::createDepthStencilTexture(uint32_t width, uint32_t height, MTL::PixelFormat format,
	                                                      uint8_t multisamplingCount) {
		MTL::TextureDescriptor* depthStencilDesc = MTL::TextureDescriptor::alloc()->init();
		depthStencilDesc->setTextureType(MTL::TextureType2DMultisample);
		depthStencilDesc->setPixelFormat(format);
		depthStencilDesc->setWidth(width);
		depthStencilDesc->setHeight(height);
		depthStencilDesc->setUsage(MTL::TextureUsageRenderTarget);
		depthStencilDesc->setSampleCount(multisamplingCount);
		m_depthStencilTexture = m_device->getDevice().value()->newTexture(depthStencilDesc);

		depthStencilDesc->release();
	}

	MetalRenderPass::~MetalRenderPass() = default;

	void MetalRenderPass::Release() {
		renderPassImages->Release();
	}

	void MetalRenderPass::RecreateRenderPassFramebuffers(uint32_t width, uint32_t height,
	                                                     std::shared_ptr<JarSurface> surface) {
		auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);
		renderPassImages->RecreateRenderPassImages(width, height, metalSurface->getDrawablePixelFormat());
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderPassImages->getMSAATexture());
		renderPassDesc->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
	}

	void MetalRenderPass::UpdateRenderPassDescriptor(std::shared_ptr<MetalSurface> metalSurface) {
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderPassImages->getMSAATexture());
		renderPassDesc->colorAttachments()->object(0)->setResolveTexture(metalSurface->getDrawable()->texture());
		renderPassDesc->depthAttachment()->setTexture(renderPassImages->getDepthStencilTexture());
	}

#pragma endregion MetalRenderPass }

#pragma region MetalBuffer{

	MetalBufferBuilder::~MetalBufferBuilder() =
	default;

	MetalBufferBuilder* MetalBufferBuilder::SetUsageFlags(BufferUsage usageFlags) {
		m_bufferUsage = std::make_optional(usageFlags);
		return this;
	}

	MetalBufferBuilder* MetalBufferBuilder::SetMemoryProperties(MemoryProperties memProps) {
		m_memoryProperties = std::make_optional(memProps);
		return this;
	}

	MetalBufferBuilder* MetalBufferBuilder::SetBufferData(const void* data, size_t bufferSize) {
		m_bufferSize = bufferSize;
		m_data = std::make_optional(data);
		return this;
	}

	std::shared_ptr<JarBuffer> MetalBufferBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		if (m_bufferSize <= 0 || !m_data.has_value() || !m_memoryProperties.has_value() ||
		    !m_bufferUsage.has_value())
			throw std::runtime_error("Could not create buffer! Provided data is insufficient.");

		const auto bufferOptions = bufferUsageToMetal(m_bufferUsage.value()) & memoryPropertiesToMetal(
				m_memoryProperties.value());
		MTL::Buffer* buffer = metalDevice->getDevice().value()->newBuffer(m_bufferSize, bufferOptions);
		memcpy(buffer->contents(), m_data.value(), m_bufferSize);
		buffer->didModifyRange(NS::Range::Make(0, buffer->length()));

		return std::make_shared<MetalBuffer>(buffer);
	}

	MTL::ResourceOptions MetalBufferBuilder::bufferUsageToMetal(const BufferUsage bufferUsage) {
		switch (bufferUsage) {
			case BufferUsage::VertexBuffer:
			case BufferUsage::IndexBuffer:
			case BufferUsage::UniformBuffer:
				return MTL::ResourceUsageRead;
			case BufferUsage::StoreBuffer:
				return MTL::ResourceUsageRead & MTL::ResourceUsageWrite;
			case BufferUsage::TransferSrc:
				return MTL::ResourceUsageRead & MTL::ResourceUsageWrite;
			case BufferUsage::TransferDest:
				return MTL::ResourceUsageRead & MTL::ResourceUsageWrite;
			default:;
		}
		return 0;
	}

	MTL::ResourceOptions MetalBufferBuilder::memoryPropertiesToMetal(const MemoryProperties memProps) {

		if (memProps & MemoryProperties::DeviceLocal)
			return MTL::StorageModePrivate;

		if (memProps & MemoryProperties::HostVisible)
			return MTL::StorageModeShared;

		if (memProps & MemoryProperties::HostCoherent)
			return MTL::StorageModeManaged;

		if (memProps & MemoryProperties::HostCached)
			return MTL::StorageModeManaged;

		if (memProps & MemoryProperties::LazilyAllocation)
			return MTL::StorageModeManaged;

		return 0;
	}

	MetalBuffer::~MetalBuffer() =
	default;

	void MetalBuffer::Release() {
		m_buffer->release();
	}

	void MetalBuffer::Update(const void* data, size_t bufferSize) {
		memcpy(m_buffer->contents(), data, bufferSize);
		m_buffer->didModifyRange(NS::Range::Make(0, m_buffer->length()));
	}

	std::optional<MTL::Buffer*> MetalBuffer::getBuffer() {
		if (m_buffer == nullptr) return std::nullopt;
		return std::make_optional(m_buffer);
	}

#pragma endregion MetalBuffer }

#pragma region MetalShader{

	MetalShaderLibraryBuilder::~MetalShaderLibraryBuilder() =
	default;

	MetalShaderLibraryBuilder* MetalShaderLibraryBuilder::SetShader(std::string shaderCode) {
		NS::String* shaderStr = NS::String::string(shaderCode.c_str(), NS::UTF8StringEncoding);
		m_shaderCodeString = std::make_optional(shaderStr);
		return this;
	}

	MetalShaderLibraryBuilder* MetalShaderLibraryBuilder::SetShaderType(ShaderType shaderType) {
		m_shaderTypeOpt = std::make_optional(shaderType);
		return this;
	}

	std::shared_ptr<JarShaderModule> MetalShaderLibraryBuilder::Build(std::shared_ptr<JarDevice> device) {
		const auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		if (!m_shaderCodeString.has_value() || !m_shaderTypeOpt.has_value())
			throw std::runtime_error("Could not build shader module! Shader type and/or code are undefined!");

		NS::Error* error = nullptr;
		const auto library = metalDevice->getDevice().value()->newLibrary(m_shaderCodeString.value(), nullptr,
		                                                                  &error);
		if (!library) {
			throw std::runtime_error("Failed to load vertex shader library: " +
			                         std::string(error->localizedDescription()->cString(NS::UTF8StringEncoding)));
		}

		return std::make_shared<MetalShaderLibrary>(library);
	}


	MetalShaderLibrary::~MetalShaderLibrary() =
	default;

	void MetalShaderLibrary::Release() {
		m_library->release();
	}

#pragma endregion }

#pragma region MetalPipeline{

	static std::unordered_map<VertexInputRate, MTL::VertexStepFunction> vertexInputRateMap{
			{VertexInputRate::PerInstance, MTL::VertexStepFunctionPerInstance},
			{VertexInputRate::PerVertex,   MTL::VertexStepFunctionPerVertex},
	};

	static std::unordered_map<Graphics::VertexFormat, MTL::VertexFormat> vertexFormatMap{
			{VertexFormat::Float,  MTL::VertexFormatFloat},
			{VertexFormat::Float2, MTL::VertexFormatFloat2},
			{VertexFormat::Float3, MTL::VertexFormatFloat3},
			{VertexFormat::Float4, MTL::VertexFormatFloat4},
			{VertexFormat::Int,    MTL::VertexFormatInt},
			{VertexFormat::Int2,   MTL::VertexFormatInt2},
			{VertexFormat::Int3,   MTL::VertexFormatInt3},
			{VertexFormat::Int4,   MTL::VertexFormatInt4},
	};

	static std::unordered_map<InputAssemblyTopology, MTL::PrimitiveTopologyClass> topologyMap{
			{InputAssemblyTopology::PointList,     MTL::PrimitiveTopologyClassPoint},
			{InputAssemblyTopology::LineList,      MTL::PrimitiveTopologyClassLine},
			{InputAssemblyTopology::LineStrip,     MTL::PrimitiveTopologyClassLine},
			{InputAssemblyTopology::TriangleList,  MTL::PrimitiveTopologyClassTriangle},
			{InputAssemblyTopology::TriangleStrip, MTL::PrimitiveTopologyClassTriangle},
	};


	static std::unordered_map<BlendFactor, MTL::BlendFactor> blendFactorMap{
			{BlendFactor::Zero,                  MTL::BlendFactorZero},
			{BlendFactor::One,                   MTL::BlendFactorOne},
			{BlendFactor::SrcColor,              MTL::BlendFactorSourceColor},
			{BlendFactor::OneMinusSrcColor,      MTL::BlendFactorOneMinusSourceColor},
			{BlendFactor::DstColor,              MTL::BlendFactorDestinationColor},
			{BlendFactor::OneMinusDstColor,      MTL::BlendFactorOneMinusDestinationColor},
			{BlendFactor::SrcAlpha,              MTL::BlendFactorSourceAlpha},
			{BlendFactor::OneMinusSrcAlpha,      MTL::BlendFactorOneMinusSourceAlpha},
			{BlendFactor::DstAlpha,              MTL::BlendFactorDestinationAlpha},
			{BlendFactor::OneMinusDstAlpha,      MTL::BlendFactorOneMinusDestinationAlpha},
			{BlendFactor::ConstantColor,         MTL::BlendFactorBlendColor},
			{BlendFactor::OneMinusConstantColor, MTL::BlendFactorOneMinusBlendColor},
			{BlendFactor::ConstantAlpha,         MTL::BlendFactorBlendAlpha},
			{BlendFactor::OneMinusConstantAlpha, MTL::BlendFactorOneMinusBlendAlpha}
	};

	static std::unordered_map<BlendOperation, MTL::BlendOperation> blendOperationMap{
			{BlendOperation::Add,             MTL::BlendOperationAdd},
			{BlendOperation::Subtract,        MTL::BlendOperationSubtract},
			{BlendOperation::ReverseSubtract, MTL::BlendOperationReverseSubtract},
			{BlendOperation::Min,             MTL::BlendOperationMin},
			{BlendOperation::Max,             MTL::BlendOperationMax},
	};

	static std::unordered_map<CompareOperation, MTL::CompareFunction> depthCompareMap{
			{CompareOperation::Never,        MTL::CompareFunctionNever},
			{CompareOperation::Less,         MTL::CompareFunctionLess},
			{CompareOperation::LessEqual,    MTL::CompareFunctionLessEqual},
			{CompareOperation::Equal,        MTL::CompareFunctionEqual},
			{CompareOperation::Greater,      MTL::CompareFunctionGreater},
			{CompareOperation::GreaterEqual, MTL::CompareFunctionGreaterEqual},
			{CompareOperation::NotEqual,     MTL::CompareFunctionNotEqual},
			{CompareOperation::AllTime,      MTL::CompareFunctionAlways}
	};

	static std::unordered_map<StencilOpState, MTL::StencilOperation> stencilOpMap{
			{StencilOpState::Keep,              MTL::StencilOperationKeep},
			{StencilOpState::Zero,              MTL::StencilOperationZero},
			{StencilOpState::Replace,           MTL::StencilOperationReplace},
			{StencilOpState::IncrementAndClamp, MTL::StencilOperationIncrementClamp},
			{StencilOpState::IncrementAndWrap,  MTL::StencilOperationIncrementWrap},
			{StencilOpState::DecrementAndClamp, MTL::StencilOperationDecrementClamp},
			{StencilOpState::DecrementAndWrap,  MTL::StencilOperationDecrementWrap},
			{StencilOpState::Invert,            MTL::StencilOperationInvert},
	};

	MetalPipelineBuilder::~MetalPipelineBuilder() =
	default;

	MetalPipelineBuilder* MetalPipelineBuilder::SetShaderStage(ShaderStage shaderStage) {
		std::string shaderMainFuncName = shaderStage.mainFunctionName + "0";
		NS::String* mainFuncName = NS::String::string(shaderMainFuncName.c_str(),
		                                              NS::StringEncoding::ASCIIStringEncoding);

		const auto metalVertexShaderModule = reinterpret_cast<std::shared_ptr<MetalShaderLibrary>&>(shaderStage.
				vertexShaderModule);
		const auto metalFragmentShaderModule = reinterpret_cast<std::shared_ptr<MetalShaderLibrary>&>(shaderStage.
				fragmentShaderModule);

		m_vertexShaderFunc = metalVertexShaderModule->getLibrary()->newFunction(mainFuncName);
		m_fragmentShaderFunc = metalFragmentShaderModule->getLibrary()->newFunction(mainFuncName);
		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = renderPass;
		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetVertexInput(VertexInput vertexInput) {
		m_vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

		for (int i = 0; i < vertexInput.attributeDescriptions.size(); ++i) {
			m_vertexDescriptor->attributes()->object(i)->setBufferIndex(
					vertexInput.attributeDescriptions[i].bindingIndex);
			m_vertexDescriptor->attributes()->object(i)->setOffset(vertexInput.attributeDescriptions[i].offset);
			m_vertexDescriptor->attributes()->object(i)->setFormat(
					vertexFormatMap[vertexInput.attributeDescriptions[i].vertexFormat]);
		}

		for (auto& bindingDescriptor: vertexInput.bindingDescriptions) {
			m_vertexDescriptor->layouts()->object(bindingDescriptor.bindingIndex)->setStepFunction(
					vertexInputRateMap[bindingDescriptor.inputRate]);
			m_vertexDescriptor->layouts()->object(bindingDescriptor.bindingIndex)->setStride(
					bindingDescriptor.stride);
			m_vertexDescriptor->layouts()->object(bindingDescriptor.bindingIndex)->setStepRate(
					bindingDescriptor.stepRate);
		}

		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetInputAssemblyTopology(InputAssemblyTopology topology) {
		m_topology = topologyMap[topology];
		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetMultisamplingCount(uint16_t multisamplingCount) {
		m_multisamplingCount = multisamplingCount;
		return this;
	}

	MetalPipelineBuilder*
	MetalPipelineBuilder::BindDescriptorLayouts(std::vector<std::shared_ptr<JarDescriptorLayout>> descriptors) {
		return this;
	}

	MetalPipelineBuilder*
	MetalPipelineBuilder::SetColorBlendAttachments(Graphics::ColorBlendAttachment blendAttachment) {
		auto colorAttachment = MTL::RenderPipelineColorAttachmentDescriptor::alloc()->init();
		colorAttachment->setPixelFormat(pixelFormatMap[blendAttachment.pixelFormat]);
		colorAttachment->setBlendingEnabled(blendAttachment.blendingEnabled);
		colorAttachment->setSourceRGBBlendFactor(blendFactorMap[blendAttachment.sourceRgbBlendFactor]);
		colorAttachment->setDestinationRGBBlendFactor(blendFactorMap[blendAttachment.destinationRgbBlendFactor]);
		colorAttachment->setRgbBlendOperation(blendOperationMap[blendAttachment.rgbBlendOperation]);
		colorAttachment->setSourceAlphaBlendFactor(blendFactorMap[blendAttachment.sourceAlphaBlendFactor]);
		colorAttachment->setDestinationAlphaBlendFactor(
				blendFactorMap[blendAttachment.destinationAlphaBlendFactor]);
		colorAttachment->setAlphaBlendOperation(blendOperationMap[blendAttachment.alphaBlendOperation]);
		colorAttachment->setWriteMask(convertToMetalColorWriteMask(blendAttachment.colorWriteMask));

		m_colorAttachments.push_back(colorAttachment);
		return this;
	}

	MetalPipelineBuilder*
	MetalPipelineBuilder::SetDepthStencilState(Graphics::DepthStencilState depthStencilState) {
		m_depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();

		if (depthStencilState.depthTestEnable) {
			m_depthStencilDescriptor->setDepthCompareFunction(depthCompareMap[depthStencilState.depthCompareOp]);
			m_depthStencilDescriptor->setDepthWriteEnabled(depthStencilState.depthWriteEnable);
		} else {
			m_depthStencilDescriptor->setDepthCompareFunction(MTL::CompareFunctionAlways);
		}

		if (depthStencilState.stencilTestEnable) {
			m_stencilDescriptor = MTL::StencilDescriptor::alloc()->init();
			m_stencilDescriptor->setStencilCompareFunction(depthCompareMap[depthStencilState.depthCompareOp]);
			m_stencilDescriptor->setStencilFailureOperation(stencilOpMap[depthStencilState.stencilFailOp]);
			m_stencilDescriptor->setDepthFailureOperation(stencilOpMap[depthStencilState.stencilDepthFailOp]);
			m_stencilDescriptor->setDepthStencilPassOperation(stencilOpMap[depthStencilState.stencilPassOp]);

			m_depthStencilDescriptor->setFrontFaceStencil(m_stencilDescriptor);
			m_depthStencilDescriptor->setBackFaceStencil(m_stencilDescriptor);
		}
		return this;
	}

	std::shared_ptr<JarPipeline> MetalPipelineBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		MTL::RenderPipelineDescriptor* metalPipelineDesc = MTL::RenderPipelineDescriptor::alloc()->init();

		if (!m_vertexShaderFunc || !m_fragmentShaderFunc)
			throw std::runtime_error("Vertex and shader function must be set to build a valid pipeline!");

		metalPipelineDesc->setVertexFunction(m_vertexShaderFunc);
		metalPipelineDesc->setFragmentFunction(m_fragmentShaderFunc);

		if (m_vertexDescriptor)
			metalPipelineDesc->setVertexDescriptor(m_vertexDescriptor);

		metalPipelineDesc->setInputPrimitiveTopology(m_topology);
		metalPipelineDesc->setSampleCount(m_multisamplingCount);

		for (int i = 0; i < m_colorAttachments.size(); ++i) {
			MTL::RenderPipelineColorAttachmentDescriptor* colorAttachment = m_colorAttachments[i];
			metalPipelineDesc->colorAttachments()->setObject(colorAttachment, i);
		}
		auto mtlDevice = metalDevice->getDevice().value();

		NS::Error* error = nullptr;
		MTL::RenderPipelineState* pipelineState = mtlDevice->newRenderPipelineState(metalPipelineDesc, &error);
		if (!pipelineState) {
			throw std::runtime_error("Failed to create render pipeline state object! " +
			                         std::string(error->localizedDescription()->utf8String()));
		}

		std::optional<MTL::DepthStencilState*> depthStencilState = std::nullopt;
		if (m_depthStencilDescriptor) {
			depthStencilState = std::make_optional(metalDevice->getDevice().value()->
					newDepthStencilState(m_depthStencilDescriptor));
		}

		std::shared_ptr<MetalPipeline> metalPipeline = std::make_shared<MetalPipeline>(mtlDevice, pipelineState,
		                                                                               depthStencilState,
		                                                                               m_renderPass);
		return metalPipeline;
	}

	MTL::ColorWriteMask MetalPipelineBuilder::convertToMetalColorWriteMask(Graphics::ColorWriteMask mask) {
		MTL::ColorWriteMask metalMask = MTL::ColorWriteMaskNone;
		auto maskValue = static_cast<std::underlying_type<ColorWriteMask>::type>(mask);

		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Red))
			metalMask |= MTL::ColorWriteMaskRed;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Green))
			metalMask |= MTL::ColorWriteMaskGreen;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Blue))
			metalMask |= MTL::ColorWriteMaskBlue;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Alpha))
			metalMask |= MTL::ColorWriteMaskAlpha;

		return metalMask;
	}

	MetalPipeline::~MetalPipeline() =
	default;

	void MetalPipeline::Release() {
		m_pipelineState->release();
	}

#pragma endregion MetalPipeline }

#pragma region MetalImage{

	MetalImageBuilder::~MetalImageBuilder() =
	default;

	MetalImageBuilder* MetalImageBuilder::SetImagePath(std::string path) {
		m_imagePath = std::make_optional(path);
		return this;
	}

	MetalImageBuilder* MetalImageBuilder::SetPixelFormat(Graphics::PixelFormat format) {
		m_pixelFormat = std::make_optional(pixelFormatMap[format]);
		return this;
	}

	MetalImageBuilder* MetalImageBuilder::EnableMipMaps(bool enabled) {
		m_enableMipMapping = enabled;
		return this;
	}

	std::shared_ptr<JarImage> MetalImageBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		if (!m_imagePath.has_value() || !m_pixelFormat.has_value())
			throw std::runtime_error("Could not create image! Image path and/or format are undefined!");

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(m_imagePath.value().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
			throw std::runtime_error("Failed to load image from path: " + m_imagePath.value());

		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(m_pixelFormat.value());
		textureDescriptor->setWidth(width);
		textureDescriptor->setHeight(height);

		uint32_t mipLevels = 1;

		if (m_enableMipMapping) {
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
			textureDescriptor->setMipmapLevelCount(mipLevels);
		}

		MTL::Texture* texture = metalDevice->getDevice().value()->newTexture(textureDescriptor);

		MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
		NS::UInteger bytesPerRow = 4 * width;

		texture->replaceRegion(region, 0, data, bytesPerRow);

		generateMipMaps(metalDevice, texture);

		textureDescriptor->release();
		stbi_image_free(data);

		MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
		samplerDescriptor->setMinFilter(MTL::SamplerMinMagFilterLinear);
		samplerDescriptor->setMagFilter(MTL::SamplerMinMagFilterLinear);
		samplerDescriptor->setMipFilter(MTL::SamplerMipFilterLinear);
		samplerDescriptor->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
		samplerDescriptor->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
		samplerDescriptor->setLodMinClamp(static_cast<float>(0));
		samplerDescriptor->setLodMaxClamp(static_cast<float>(mipLevels + 1));

		MTL::SamplerState* sampler = metalDevice->getDevice().value()->newSamplerState(samplerDescriptor);
		samplerDescriptor->release();


		return std::make_shared<MetalImage>(texture, sampler);
	}

	void MetalImageBuilder::generateMipMaps(std::shared_ptr<MetalDevice>& device, MTL::Texture* texture) {
		auto queueBuilder = new MetalCommandQueueBuilder();
		auto queue = queueBuilder->SetCommandBufferAmount(1)->Build(device);

		auto commandBuffer = reinterpret_cast<MetalCommandBuffer*>(queue->getNextCommandBuffer());
		auto blitCommandEncoder = commandBuffer->getCommandBuffer()->blitCommandEncoder();
		blitCommandEncoder->generateMipmaps(texture);
		blitCommandEncoder->endEncoding();
		commandBuffer->getCommandBuffer()->addCompletedHandler([](MTL::CommandBuffer* buffer) {
			buffer->release();
		});
		commandBuffer->getCommandBuffer()->commit();

	}

	MetalImage::~MetalImage() =
	default;

	void MetalImage::Release() {
		m_texture->release();
	}


#pragma endregion MetalImage }

#pragma region MetalDescriptorBinding{

	MetalDescriptorBuilder::~MetalDescriptorBuilder() = default;

	MetalDescriptorBuilder* MetalDescriptorBuilder::SetBinding(uint32_t binding) {
		m_binding = std::make_optional(binding);
		return this;
	}

	MetalDescriptorBuilder* MetalDescriptorBuilder::SetStageFlags(Graphics::StageFlags stageFlags) {
		m_stageFlags = std::make_optional(stageFlags);
		return this;
	}

	std::shared_ptr<JarDescriptor>
	MetalDescriptorBuilder::BuildUniformBufferDescriptor(std::shared_ptr<JarDevice> device,
	                                                     std::vector<std::shared_ptr<JarBuffer>> uniformBuffers) {
		if (!m_binding.has_value() || !m_stageFlags.has_value())
			throw std::runtime_error("Could not create descriptor! Binding and/or stage flags are undefined!");

		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		std::vector<std::shared_ptr<MetalBuffer>> metalBuffers;
		for (auto buffer: uniformBuffers) {
			auto metalBuffer = reinterpret_cast<std::shared_ptr<MetalBuffer>&>(buffer);
			metalBuffers.push_back(metalBuffer);
		}

		auto content = std::make_shared<MetalUniformDescriptorContent>(metalBuffers);
		auto layout = std::make_shared<MetalDescriptorLayout>();

		return std::make_shared<MetalDescriptor>(m_binding.value(), m_stageFlags.value(), content, layout);
	}

	std::shared_ptr<JarDescriptor> MetalDescriptorBuilder::BuildImageBufferDescriptor(std::shared_ptr<JarDevice> device,
	                                                                                  std::shared_ptr<JarImage> image) {

		if (!m_binding.has_value() || !m_stageFlags.has_value())
			throw std::runtime_error("Could not create descriptor! Binding and/or stage flags are undefined!");

		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		auto metalImage = reinterpret_cast<std::shared_ptr<MetalImage>&>(image);

		auto content = std::make_shared<MetalImageDescriptorContent>(metalImage);
		auto layout = std::make_shared<MetalDescriptorLayout>();

		return std::make_shared<MetalDescriptor>(m_binding.value(), m_stageFlags.value(), content, layout);
	}

	MetalDescriptorLayout::~MetalDescriptorLayout() = default;

	void MetalDescriptorLayout::Release() {}

	MetalDescriptor::~MetalDescriptor() = default;

	void MetalDescriptor::Release() {}

	std::shared_ptr<JarDescriptorLayout> MetalDescriptor::GetDescriptorLayout() { return m_descriptorLayout; }

	void MetalDescriptor::BindContentToEncoder(MTL::RenderCommandEncoder* encoder) {
		m_content->BindContentToEncoder(encoder, m_binding, m_stageFlags);
	}

	MetalUniformDescriptorContent::~MetalUniformDescriptorContent() = default;

	void MetalUniformDescriptorContent::Release() {}

	void MetalUniformDescriptorContent::BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding,
	                                                         Graphics::StageFlags stageFlags) {
		if (stageFlags != StageFlags::VertexShader)
			std::runtime_error("Uniform buffer can only be bound to the vertex shader!");

		encoder->setVertexBuffer(m_uniformBuffers[m_currentBufferIndex]->getBuffer().value(), 0, binding);

		m_currentBufferIndex = (m_currentBufferIndex + 1) % m_uniformBuffers.size();
	}

	MetalImageDescriptorContent::~MetalImageDescriptorContent() = default;

	void MetalImageDescriptorContent::Release() {}

	void MetalImageDescriptorContent::BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding,
	                                                       Graphics::StageFlags stageFlags) {
		if (stageFlags != StageFlags::FragmentShader)
			std::runtime_error("Image buffer can only be bound to the fragment shader!");

		encoder->setFragmentTexture(m_image->getTexture(), binding);
		encoder->setFragmentSamplerState(m_image->getSampler(), binding);

	}

#pragma endregion MetalDescriptorBinding }
}
#endif
