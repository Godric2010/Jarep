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

	JarBuffer *MetalDevice::CreateBuffer(size_t bufferSize, const void *data) {
		auto metalBuffer = new MetalBuffer();
		metalBuffer->CreateBuffer(bufferSize, data, _device.value());
		return metalBuffer;
	}

	JarShaderModule *MetalDevice::CreateShaderModule(std::string shaderContent) {
		auto shaderLib = new MetalShaderLibrary();
		shaderLib->CreateShaderLibrary(_device.value(), shaderContent);
		return shaderLib;
	}

	JarPipeline *MetalDevice::CreatePipeline(JarShaderModule *vertexModule, JarShaderModule *fragmentModule) {

		auto *vertexShaderLib = reinterpret_cast<MetalShaderLibrary *>(vertexModule);
		auto *fragmentShaderLib = reinterpret_cast<MetalShaderLibrary *>(fragmentModule);

		auto pso = new MetalPipeline();
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

	void MetalCommandBuffer::BindPipeline(Graphics::JarPipeline *pipeline) {
		auto *metalPipeline = reinterpret_cast<MetalPipeline *>(pipeline);
		encoder->setRenderPipelineState(metalPipeline->getPSO());
	}

	void MetalCommandBuffer::BindVertexBuffer(Graphics::JarBuffer *buffer) {
		auto *metalBuffer = reinterpret_cast<MetalBuffer *>(buffer);
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


#pragma region MetalAPI {

	MetalAPI::MetalAPI() {
	}

	MetalAPI::~MetalAPI() {
	}

	void MetalAPI::RegisterPhysicalDevice() {
		device = MTL::CreateSystemDefaultDevice();
	}

	void MetalAPI::CreateLogicalDevice() {
		surface->setDevice(device);
		window->setContentView(surface);
	}

	void MetalAPI::CreateSurface(NativeWindowHandleProvider *nativeWindowHandle) {
		window = reinterpret_cast<NS::Window *>(nativeWindowHandle->getNativeWindowHandle());

		CGRect surfaceRect = CGRectMake(0, 0, nativeWindowHandle->getWindowWidth(),
		                                nativeWindowHandle->getWindowHeight());

		surface = MTK::View::alloc()->init(surfaceRect, device);

		surface->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		surface->setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 0.0f, 1.0f));
		std::cout << "Creating a cocoa surface" << std::endl;
	}

	void MetalAPI::CreateVertexBuffer() {
		const std::vector<Vertex> vertices = {
				{{0.0f,  -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}}
		};

		const size_t vertexDataSize = vertices.size() * sizeof(Vertex);
		triangleVertexBuffer = device->newBuffer(vertexDataSize, MTL::ResourceStorageModeManaged);
		memcpy(triangleVertexBuffer->contents(), vertices.data(), vertexDataSize);
		triangleVertexBuffer->didModifyRange(NS::Range::Make(0, triangleVertexBuffer->length()));
	}

	void MetalAPI::CreateShaders() {
		const auto vertexShaderSrc = readFile("shaders/triangle_vert.metal");
		const auto fragmentShaderSrc = readFile("shaders/triangle_frag.metal");

		const NS::String *vertShaderStr = NS::String::string(vertexShaderSrc.c_str(), NS::UTF8StringEncoding);
		const NS::String *fragShaderStr = NS::String::string(fragmentShaderSrc.c_str(), NS::UTF8StringEncoding);

		NS::Error *error = nullptr;
		vertShaderLibrary = device->newLibrary(vertShaderStr, nullptr, &error);
		if (!vertShaderLibrary) {
			throw std::runtime_error("Failed to load vertex shader library: " +
			                         std::string(error->localizedDescription()->cString(NS::UTF8StringEncoding)));
		}

		fragShaderLibrary = device->newLibrary(fragShaderStr, nullptr, &error);
		if (!fragShaderLibrary) {
			throw std::runtime_error("Failed to load fragment shader library: " +
			                         std::string(error->localizedDescription()->cString(NS::UTF8StringEncoding)));
		}
	}

	void MetalAPI::CreateCommandQueue() {
		commandQueue = device->newCommandQueue();
	}

	void MetalAPI::CreateGraphicsPipeline() {
		MTL::Function *vertexShader = vertShaderLibrary->newFunction(
				NS::String::string("main0", NS::ASCIIStringEncoding));
		assert(vertexShader);
		MTL::Function *fragmentShader = fragShaderLibrary->
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
		renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(metalLayer->pixelFormat());
		renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

		NS::Error *error = nullptr;
		renderPSO = device->newRenderPipelineState(renderPipelineDescriptor, &error);
		if (!renderPSO) {
			throw std::runtime_error("Failed to create render pipeline state object! " +
			                         std::string(error->localizedDescription()->utf8String()));
		}
		renderPipelineDescriptor->release();

		//		MTL::RenderPassDescriptor *rpd = MTL::RenderPassDescriptor::alloc()->init();
		//		MTL::RenderPassColorAttachmentDescriptor *cd = rpd->colorAttachments()->object(0);
		//		cd->setTexture(metalDrawable->texture());
		//		cd->setLoadAction(MTL::LoadActionClear);
		//		cd->setClearColor(MTL::ClearColor(0.0, 0.0, 0.0, 1.0));
		//		cd->setStoreAction(MTL::StoreActionStore);
		//		renderPassDescriptor = rpd;
	}

	void MetalAPI::RecordCommandBuffer() {
	}

	void MetalAPI::Draw() {
		NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

		//		metalDrawable = surface->currentDrawable();
		commandBuffer = commandQueue->commandBuffer();
		MTL::RenderCommandEncoder *renderCommandEncoder = commandBuffer->renderCommandEncoder(
				surface->currentRenderPassDescriptor());
		//		renderCommandEncoder->setRenderPipelineState(renderPSO);
		//		renderCommandEncoder->setVertexBuffer(triangleVertexBuffer, 0, 0);
		//		renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
		renderCommandEncoder->endEncoding();

		commandBuffer->presentDrawable(surface->currentDrawable());
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		pool->release();
	}

	void MetalAPI::Shutdown() {
		commandQueue->release();
		device->release();
	}

	std::string MetalAPI::readFile(const std::string &filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!std::filesystem::exists(filename)) {
			throw std::runtime_error("File does not exist: " + filename);
		}

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filename);
		}

		auto fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		std::string bufferString(buffer.begin(), buffer.end());
		return bufferString;
	}

	void MetalAPI::encodeRenderCommand(MTL::RenderCommandEncoder *renderCommandEncoder) {
		renderCommandEncoder->setRenderPipelineState(renderPSO);
		renderCommandEncoder->setVertexBuffer(triangleVertexBuffer, 0, 0);
		renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
	}

#pragma endregion MetalAPI }

}
#endif
