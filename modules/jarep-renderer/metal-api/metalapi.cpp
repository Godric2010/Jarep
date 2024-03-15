//
// Created by Sebastian Borsch on 24.10.23.
//
#include <iostream>
#include <utility>

#if defined(__APPLE__)

#include "metalapi.hpp"
#include "Vertex.hpp"

namespace Graphics::Metal {
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
		metalSurface->FinalizeSurface(metalDevice->getDevice().value());

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

	void MetalSurface::Update() {
	}

	void MetalSurface::ReleaseSwapchain() {

	}

	uint32_t MetalSurface::GetSwapchainImageAmount() {
		return maxSwapchainImageCount;
	}

	JarExtent MetalSurface::GetSurfaceExtent() {
		return JarExtent{static_cast<float>(surfaceRect.size.width), static_cast<float>(surfaceRect.size.height)};
	}

	void MetalSurface::FinalizeSurface(MTL::Device* device) {
		Graphics::Metal::SDLSurfaceAdapter::CreateViewAndMetalLayer(surfaceRect, &contentView, &layer,
		                                                            &maxSwapchainImageCount);

		if (contentView == nullptr)
			throw std::runtime_error("Expected NS::View* to be not nullptr!");

		if (layer == nullptr)
			throw std::runtime_error("Expected metal layer not to be nullptr");

		layer->setDevice(device);
		layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

		window->setContentView(contentView);

		createDepthStencilTexture(device);
	}

	void MetalSurface::createDepthStencilTexture(MTL::Device* device) {
		MTL::TextureDescriptor* depthStencilDesc = MTL::TextureDescriptor::alloc()->init();
		depthStencilDesc->setTextureType(MTL::TextureType2D);
		depthStencilDesc->setPixelFormat(MTL::PixelFormatDepth32Float);
		depthStencilDesc->setWidth(surfaceRect.size.width);
		depthStencilDesc->setHeight(surfaceRect.size.height);
		depthStencilDesc->setUsage(MTL::TextureUsageRenderTarget);
		m_depthStencilTexture = device->newTexture(depthStencilDesc);

		depthStencilDesc->release();
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

	void
	MetalCommandBuffer::StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) {
		const auto metalRenderPass = reinterpret_cast<std::shared_ptr<MetalRenderPass>&>(renderPass);
		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);

		auto renderPassDesc = metalRenderPass->getRenderPassDesc();
		renderPassDesc->colorAttachments()->object(0)->setTexture(metalSurface->acquireNewDrawTexture());
		encoder = buffer->renderCommandEncoder(renderPassDesc);
	}

	void MetalCommandBuffer::EndRecording() {
		encoder->endEncoding();
	}

	void MetalCommandBuffer::BindPipeline(std::shared_ptr<Graphics::JarPipeline> pipeline, uint32_t frameIndex) {
		auto metalPipeline = reinterpret_cast<MetalPipeline*>(pipeline.get());
		encoder->setRenderPipelineState(metalPipeline->getPSO());
		encoder->setDepthStencilState(metalPipeline->getDSS());

		for (auto& uniformDescriptorBinding: metalPipeline->getUniformDescriptorBindings()) {
			if (uniformDescriptorBinding.getStageFlags() == Graphics::StageFlags::VertexShader)
				encoder->setVertexBuffer(uniformDescriptorBinding.getUniformBuffer(frameIndex)->getBuffer().value(), 0,
				                         uniformDescriptorBinding.getBinding());
		}

		for (auto& textureDescriptorBinding: metalPipeline->getTextureDescriptorBindings()) {
			if (textureDescriptorBinding.getStageFlags() == Graphics::StageFlags::FragmentShader)
				encoder->setFragmentTexture(textureDescriptorBinding.getImage()->getTexture(),
				                            textureDescriptorBinding.getBinding());
		}
	}

	void MetalCommandBuffer::BindVertexBuffer(std::shared_ptr<Graphics::JarBuffer> buffer) {
		auto* metalBuffer = reinterpret_cast<MetalBuffer*>(buffer.get());
		encoder->setVertexBuffer(metalBuffer->getBuffer().value(), 0, 0);
	}

	void MetalCommandBuffer::BindIndexBuffer(std::shared_ptr<JarBuffer> buffer) {
		std::shared_ptr<MetalBuffer> metalBuffer = reinterpret_cast<std::shared_ptr<MetalBuffer>&>(buffer);
		indexBuffer = metalBuffer;
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
		buffer->presentDrawable(metalSurface->getDrawable());
		buffer->commit();
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

		useDepthAttachment = true;

		return this;

	}

	std::shared_ptr<JarRenderPass>
	MetalRenderPassBuilder::Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) {
		if (!m_colorAttachment.has_value())
			throw std::exception();

		auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface>&>(surface);

		if (useDepthAttachment)
			m_renderPassDescriptor->depthAttachment()->setTexture(metalSurface->getDepthStencilTexture());
		return std::make_shared<MetalRenderPass>(m_renderPassDescriptor);
	}

	MetalRenderPass::~MetalRenderPass() =
	default;

	void MetalRenderPass::Release() {
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

	static std::unordered_map<PixelFormat, MTL::PixelFormat> pixelFormatMap{
			{PixelFormat::BC1,              MTL::PixelFormatBC1_RGBA},
			{PixelFormat::BC3,              MTL::PixelFormatBC3_RGBA},
			{PixelFormat::BGRA8_UNORM,      MTL::PixelFormat::PixelFormatBGRA8Unorm},
			{PixelFormat::RGBA8_UNORM,      MTL::PixelFormatRGBA8Unorm},
			{PixelFormat::DEPTH24_STENCIL8, MTL::PixelFormatDepth24Unorm_Stencil8},
			{PixelFormat::DEPTH32_FLOAT,    MTL::PixelFormatDepth32Float},
			{PixelFormat::PVRTC,            MTL::PixelFormatPVRTC_RGBA_4BPP},
			{PixelFormat::RGBA16_FLOAT,     MTL::PixelFormatRGBA16Float},
			{PixelFormat::RGBA32_FLOAT,     MTL::PixelFormatRGBA32Float},
			{PixelFormat::R16_FLOAT,        MTL::PixelFormatR16Float},
			{PixelFormat::R8_UNORM,         MTL::PixelFormatR8Unorm}
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

	static std::unordered_map<DepthCompareOperation, MTL::CompareFunction> depthCompareMap{
			{DepthCompareOperation::Never,        MTL::CompareFunctionNever},
			{DepthCompareOperation::Less,         MTL::CompareFunctionLess},
			{DepthCompareOperation::LessEqual,    MTL::CompareFunctionLessEqual},
			{DepthCompareOperation::Equal,        MTL::CompareFunctionEqual},
			{DepthCompareOperation::Greater,      MTL::CompareFunctionGreater},
			{DepthCompareOperation::GreaterEqual, MTL::CompareFunctionGreaterEqual},
			{DepthCompareOperation::NotEqual,     MTL::CompareFunctionNotEqual},
			{DepthCompareOperation::AllTime,      MTL::CompareFunctionAlways}
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
		const auto mainFunc = (shaderStage.mainFunctionName + "0").c_str();
		NS::String* mainFuncName = NS::String::string(mainFunc, NS::StringEncoding::ASCIIStringEncoding);

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

	MetalPipelineBuilder* MetalPipelineBuilder::BindUniformBuffers(
			std::vector<std::shared_ptr<JarBuffer>> uniformBuffers, uint32_t binding,
			Graphics::StageFlags stageFlags) {

		auto metalUniformBuffers = reinterpret_cast<std::vector<std::shared_ptr<MetalBuffer>>&>(uniformBuffers);
		m_uniformDescriptorBindings.emplace_back(metalUniformBuffers, binding, stageFlags);

		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::BindImageBuffer(std::shared_ptr<JarImage> image, uint32_t binding,
	                                                            Graphics::StageFlags stageFlags) {

		auto metalImage = reinterpret_cast<std::shared_ptr<MetalImage>&>(image);
		m_textureDescriptorBindings.emplace_back(metalImage, binding, stageFlags);
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
			m_stencilDescriptor->setStencilFailureOperation(stencilOpMap[depthStencilState.stencilOpState]);
			m_stencilDescriptor->setDepthFailureOperation(stencilOpMap[depthStencilState.stencilOpState]);
			m_stencilDescriptor->setDepthStencilPassOperation(stencilOpMap[depthStencilState.stencilOpState]);

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

		MTL::DepthStencilState* depthStencilState = nullptr;
		if (m_depthStencilDescriptor) {
			depthStencilState = metalDevice->getDevice().value()->newDepthStencilState(m_depthStencilDescriptor);
		}

		auto metalPipeline = std::make_shared<MetalPipeline>(mtlDevice, pipelineState, depthStencilState,
		                                                     m_renderPass, m_uniformDescriptorBindings,
		                                                     m_textureDescriptorBindings);
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

		MTL::Texture* texture = metalDevice->getDevice().value()->newTexture(textureDescriptor);

		MTL::Region region = MTL::Region::Make2D(0, 0, width, height);
		NS::UInteger bytesPerRow = 4 * width;

		texture->replaceRegion(region, 0, data, bytesPerRow);
		textureDescriptor->release();
		stbi_image_free(data);

		return std::make_shared<MetalImage>(texture);
	}

	MetalImage::~MetalImage() =
	default;

	void MetalImage::Release() {
		m_texture->release();
	}


#pragma endregion MetalImage }

}
#endif
