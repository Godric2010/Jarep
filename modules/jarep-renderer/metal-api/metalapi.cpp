//
// Created by Sebastian Borsch on 24.10.23.
//

#include "metalapi.hpp"

namespace Graphics::Metal {

	MetalAPI::MetalAPI() {

	}

	MetalAPI::~MetalAPI(){
//		commandQueue->release();
//		device->release();
	}

	void MetalAPI::CreateDevice() {
		device = MTL::CreateSystemDefaultDevice();
	}

	void MetalAPI::CreateSurface(void *nativeWindowHandle, int surfaceWidth, int surfaceHeight) {
		window = reinterpret_cast<NS::Window*>(nativeWindowHandle);

		CGRect surfaceRect = CGRectMake(0,0, surfaceWidth, surfaceHeight);

		surface = MTK::View::alloc()->init(surfaceRect, device);
		surface->setDevice(device);
		surface->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		surface->setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 0.0f, 1.0f));

		window->setContentView(surface);
		std::cout << "Creating a cocoa surface" << std::endl;
	}

	void MetalAPI::CreateVertexBuffer() {
	}

	void MetalAPI::CreateShaders() {

	}

	void MetalAPI::CreateCommandQueue() {
		commandQueue = device->newCommandQueue();
	}

	void MetalAPI::CreateGraphicsPipeline() {

	}

	void MetalAPI::RecordCommandBuffer() {

	}

	void MetalAPI::Draw() {
		NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

		MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
		MTL::RenderPassDescriptor* renderPassDescriptor = surface->currentRenderPassDescriptor();
		MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor );
		renderCommandEncoder->endEncoding();
		commandBuffer->presentDrawable(surface->currentDrawable() );
		commandBuffer->commit();

		pool->release();
	}


}