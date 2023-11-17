//
// Created by Sebastian Borsch on 24.10.23.
//
#if defined(__APPLE__)

#include "metalapi.hpp"
#include "Vertex.hpp"

namespace Graphics::Metal {
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
