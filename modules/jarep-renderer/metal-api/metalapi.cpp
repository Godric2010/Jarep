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

	std::shared_ptr<JarSurface> MetalBackend::CreateSurface(NativeWindowHandleProvider *windowHandleProvider) {
		auto metalSurface = std::make_shared<MetalSurface>();
		metalSurface->CreateFromNativeWindowProvider(windowHandleProvider);
		return metalSurface;
	}

	std::shared_ptr<JarDevice> MetalBackend::CreateDevice(std::shared_ptr<JarSurface> &surface) {
		auto metalDevice = std::make_shared<MetalDevice>();
		metalDevice->Initialize();

		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface> &>(surface);
		metalSurface->FinalizeSurface(metalDevice->getDevice().value());

		return metalDevice;
	}


#pragma endregion MetalBackend }

#pragma region MetalSurface{

	MetalSurface::MetalSurface() = default;

	MetalSurface::~MetalSurface() = default;


	bool MetalSurface::CreateFromNativeWindowProvider(NativeWindowHandleProvider *windowHandleProvider) {
		window = static_cast<NS::Window *>(windowHandleProvider->getNativeWindowHandle());


		surfaceRect = CGRectMake(0, 0, windowHandleProvider->getWindowWidth(),
		                         windowHandleProvider->getWindowHeight());
		return true;
	}

	void MetalSurface::Update() {
	}

	JarRenderPass *MetalSurface::CreateRenderPass() {
		if (contentView == nullptr) {
			throw std::runtime_error("Surface has not been initialized!");
		}
		drawable = layer->nextDrawable();
		MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();;
		MTL::RenderPassColorAttachmentDescriptor *cd = renderPassDescriptor->colorAttachments()->object(0);
		cd->setTexture(drawable->texture());
		cd->setLoadAction(MTL::LoadActionClear);
		cd->setClearColor(MTL::ClearColor(0.0, 0.0, 0.0, 1.0));
		cd->setStoreAction(MTL::StoreActionStore);

		return new MetalRenderPass(renderPassDescriptor);
	}


	void MetalSurface::FinalizeSurface(MTL::Device *device) {

		Graphics::Metal::SDLSurfaceAdapter::CreateViewAndMetalLayer(surfaceRect, &contentView, &layer);

		if (contentView == nullptr)
			throw std::runtime_error("Expected NS::View* to be not nullptr!");

		if (layer == nullptr)
			throw std::runtime_error("Expected metal layer not to be nullptr");

		layer->setDevice(device);
		layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

		window->setContentView(contentView);
	}


#pragma endregion MetalSurface }

#pragma region MetalDevice {

	MetalDevice::~MetalDevice() = default;

	void MetalDevice::Initialize() {
		_device = std::make_optional(MTL::CreateSystemDefaultDevice());
	}

	std::optional<MTL::Device *> MetalDevice::getDevice() const {
		return _device;
	}

	void MetalDevice::Release() {
		if (!_device.has_value()) return;
		_device.value()->release();
	}

	std::shared_ptr<JarCommandQueue> MetalDevice::CreateCommandQueue() {
		const auto cmdQueue = _device.value()->newCommandQueue();
		const auto metalQueue = std::make_shared<MetalCommandQueue>(cmdQueue);
		return metalQueue;
	}

	std::shared_ptr<JarBuffer> MetalDevice::CreateBuffer(size_t bufferSize, const void *data) {
		auto metalBuffer = std::make_shared<MetalBuffer>();
		metalBuffer->CreateBuffer(bufferSize, data, _device.value());
		return metalBuffer;
	}

	std::shared_ptr<JarShaderModule> MetalDevice::CreateShaderModule(std::string shaderContent) {
		auto shaderLib = std::make_shared<MetalShaderLibrary>();
		shaderLib->CreateShaderLibrary(_device.value(), std::move(shaderContent));
		return shaderLib;
	}

	std::shared_ptr<JarPipeline> MetalDevice::CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule,
	                                                         std::shared_ptr<JarShaderModule> fragmentModule) {

		auto *vertexShaderLib = reinterpret_cast<MetalShaderLibrary *>(vertexModule.get());
		auto *fragmentShaderLib = reinterpret_cast<MetalShaderLibrary *>(fragmentModule.get());

		auto pso = std::make_shared<MetalPipeline>();
		pso->CreatePipeline(_device.value(), vertexShaderLib->getLibrary(), fragmentShaderLib->getLibrary());

		return pso;

	}

#pragma endregion MetalDevice }

#pragma region MetalCommandQueue {

	MetalCommandQueue::~MetalCommandQueue() {
	}

	JarCommandBuffer *MetalCommandQueue::getNextCommandBuffer() {
		return new MetalCommandBuffer(queue->commandBuffer());
	}

	void MetalCommandQueue::Release() {
		queue->release();
	}

#pragma endregion MetalComandQueue }

#pragma region MetalCommandBuffer {

	MetalCommandBuffer::~MetalCommandBuffer() = default;

	void MetalCommandBuffer::StartRecording(JarRenderPass *renderPass) {
		const auto metalRenderPass = reinterpret_cast<MetalRenderPass *>(renderPass);
		encoder = buffer->renderCommandEncoder(metalRenderPass->getRenderPassDesc());
	}

	void MetalCommandBuffer::EndRecording() {
		encoder->endEncoding();
	}

	void MetalCommandBuffer::BindPipeline(std::shared_ptr<Graphics::JarPipeline> pipeline) {
		auto metalPipeline = reinterpret_cast<MetalPipeline *>(pipeline.get());
		encoder->setRenderPipelineState(metalPipeline->getPSO());
	}

	void MetalCommandBuffer::BindVertexBuffer(std::shared_ptr<Graphics::JarBuffer> buffer) {
		auto *metalBuffer = reinterpret_cast<MetalBuffer *>(buffer.get());
		encoder->setVertexBuffer(metalBuffer->getBuffer().value(), 0, 0);

	}

	void MetalCommandBuffer::Draw() {
		encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

	}

	void MetalCommandBuffer::Present(std::shared_ptr<JarSurface> &surface) {
		const auto metalSurface = reinterpret_cast<std::shared_ptr<MetalSurface> &>(surface);
		buffer->presentDrawable(metalSurface->getDrawable());
		buffer->commit();
	}


#pragma endregion MetalCommandBuffer }

#pragma region MetalRenderPass{

	MetalRenderPass::~MetalRenderPass() {
	}


#pragma endregion MetalRenderPass }

#pragma region MetalBuffer{

	MetalBuffer::~MetalBuffer() = default;

	void MetalBuffer::CreateBuffer(size_t size, const void *data, MTL::Device *metalDevice) {
		buffer = metalDevice->newBuffer(size, MTL::ResourceStorageModeManaged);
		memcpy(buffer->contents(), data, size);
		buffer->didModifyRange(NS::Range::Make(0, buffer->length()));
	}

	std::optional<MTL::Buffer *> MetalBuffer::getBuffer() {
		if (buffer == nullptr) return std::nullopt;
		return std::make_optional(buffer);
	}

#pragma endregion MetalBuffer }

#pragma region MetalShader{

	MetalShaderLibrary::~MetalShaderLibrary() = default;

	void MetalShaderLibrary::CreateShaderLibrary(MTL::Device *device, std::string shaderContent) {

		const NS::String *shaderStr = NS::String::string(shaderContent.c_str(), NS::UTF8StringEncoding);

		NS::Error *error = nullptr;
		library = device->newLibrary(shaderStr, nullptr, &error);
		if (!library) {
			throw std::runtime_error("Failed to load vertex shader library: " +
			                         std::string(error->localizedDescription()->cString(NS::UTF8StringEncoding)));
		}
	}

	void MetalShaderLibrary::Release() {
		library->release();
	}

#pragma endregion }

#pragma region MetalPipeline{

	MetalPipeline::~MetalPipeline() = default;

	void MetalPipeline::CreatePipeline(MTL::Device *device, MTL::Library *vertexLib, MTL::Library *fragmentLib) {
		MTL::Function *vertexShader = vertexLib->newFunction(
				NS::String::string("main0", NS::ASCIIStringEncoding));
		assert(vertexShader);
		MTL::Function *fragmentShader = fragmentLib->
				newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
		assert(fragmentShader);

		auto vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
		vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat3);
		vertexDescriptor->attributes()->object(0)->setOffset(0);
		vertexDescriptor->attributes()->object(0)->setBufferIndex(0);

		vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat3);
		vertexDescriptor->attributes()->object(1)->setOffset(sizeof(float) * 3);
		vertexDescriptor->attributes()->object(1)->setBufferIndex(0);

		vertexDescriptor->layouts()->object(0)->setStride(sizeof(float) * 6);
		vertexDescriptor->layouts()->object(0)->setStepFunction(MTL::VertexStepFunctionPerVertex);

		MTL::RenderPipelineDescriptor *renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
		renderPipelineDescriptor->setLabel(NS::String::string("Triangle rendering pipeline", NS::ASCIIStringEncoding));
		renderPipelineDescriptor->setVertexFunction(vertexShader);
		renderPipelineDescriptor->setFragmentFunction(fragmentShader);
		renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(
				MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB/*metalLayer->pixelFormat()*/);
		renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

		NS::Error *error = nullptr;
		pipelineState = device->newRenderPipelineState(renderPipelineDescriptor, &error);
		if (!pipelineState) {
			throw std::runtime_error("Failed to create render pipeline state object! " +
			                         std::string(error->localizedDescription()->utf8String()));
		}
	}

	void MetalPipeline::Release() {
		pipelineState->release();
	}

#pragma endregion MetalPipeline }

}
#endif
