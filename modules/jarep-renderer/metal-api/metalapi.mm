//
// Created by Sebastian Borsch on 24.10.23.
//

#include "metalapi.hpp"

#include <Metal/Metal.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/QuartzCore.h>
#import <simd/simd.h>

namespace Graphics::Metal {

	struct MDevice {
		id <MTLDevice> device;
	};

	struct MWindow {
		NSWindow *metalWindow;
		CAMetalLayer *metalLayer;
		id<CAMetalDrawable> metalDrawable;

	};

	struct MLibrary {
		id <MTLLibrary> library;
	};

	struct MCommandQueue {
		id <MTLCommandQueue> queue;
	};

	struct MCommandBuffer {
		id <MTLCommandBuffer> commandBuffer;
	};

	struct MRenderPipeline {
		id <MTLRenderPipelineState> pipelineState;
	};

	struct MBuffer {
		id <MTLBuffer> buffer;
	};

	MetalAPI::MetalAPI() {
		device = new MDevice();
		window = new MWindow();
		library = new MLibrary();
		cmdQueue = new MCommandQueue();
		cmdBuffer = new MCommandBuffer();
		pipeline = new MRenderPipeline();
		buffer = new MBuffer();
	}

	MetalAPI::~MetalAPI() {

		[device->device release];

//		delete window;
//		delete device;
	}

	void MetalAPI::CreateDevice() {
		auto metalDevice = MTLCreateSystemDefaultDevice();
		if (!metalDevice) {
			fprintf(stderr, "No device supporting metal was found");
			abort();
		}
		device->device = metalDevice;
	}

	void MetalAPI::CreateSurface(void *nativeWindowHandle) {
		auto windowHandle = (__bridge NSWindow *) nativeWindowHandle;
		auto metalLayer = [CAMetalLayer layer];
		metalLayer.device = device->device;
		metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
		windowHandle.contentView.layer = metalLayer;
		windowHandle.contentView.wantsLayer = YES;

		window->metalLayer = metalLayer;
		window->metalWindow = windowHandle;

		std::cout << "Creating a cocoa surface" << std::endl;
	}

	void MetalAPI::CreateVertexBuffer() {
		simd::float3 triangleVertices[] = {
				{-0.5f, -0.5f, 0.0f},
				{0.5f,  -0.5f, 0.0f},
				{0.0f,  0.5f,  0.0f}
		};
		buffer->buffer = [device->device newBufferWithBytes:&triangleVertices length:sizeof(triangleVertices) options:MTLResourceStorageModeShared];
	}

	void MetalAPI::CreateShaders() {
		auto defaultLibrary = [device->device newDefaultLibrary];
		if (!defaultLibrary) {
			std::cout << "Failed to load default library" << std::endl;
			abort();
		}
		library->library = defaultLibrary;
	}

	void MetalAPI::CreateCommandQueue() {
		cmdQueue->queue = [device->device newCommandQueue];
	}

	void MetalAPI::CreateGraphicsPipeline() {
		auto vertShader = [library->library newFunctionWithName:@"vertexShader"];
		assert(vertShader);
		auto fragShader = [library->library newFunctionWithName:@"fragmentShader"];
		assert(fragShader);

		auto renderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
		renderPipelineDesc.label = @"Test render pipeline";
		renderPipelineDesc.vertexFunction = vertShader;
		renderPipelineDesc.fragmentFunction = fragShader;
		assert(renderPipelineDesc);

		renderPipelineDesc.colorAttachments[0].pixelFormat = window->metalLayer.pixelFormat;

		NSError* error;
		pipeline->pipelineState = [device->device newRenderPipelineStateWithDescriptor:renderPipelineDesc error:&error];

		[renderPipelineDesc release];
	}

	void MetalAPI::RecordCommandBuffer() {
		cmdBuffer->commandBuffer = [cmdQueue->queue commandBuffer];
		auto renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
		auto renderPassColorAttachmentDescriptor = [renderPassDescriptor.colorAttachments objectAtIndexedSubscript:0];
		[renderPassColorAttachmentDescriptor setTexture:window->metalDrawable.texture];
		[renderPassColorAttachmentDescriptor setLoadAction:MTLLoadAction::MTLLoadActionClear];
		[renderPassColorAttachmentDescriptor setClearColor:MTLClearColor{41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0}];
		[renderPassColorAttachmentDescriptor setStoreAction: MTLStoreAction::MTLStoreActionStore];

		auto renderCommandEncoder = [cmdBuffer->commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
		[renderCommandEncoder setRenderPipelineState:pipeline->pipelineState];
		[renderCommandEncoder setVertexBuffer:buffer->buffer offset:0 atIndex:0];
		NSUInteger vertStart = 0;
		NSUInteger vertCount = 3;
		[renderCommandEncoder drawPrimitives: MTLPrimitiveType::MTLPrimitiveTypeTriangle vertexStart:vertStart vertexCount: vertCount];
		[renderCommandEncoder endEncoding];

		[cmdBuffer->commandBuffer presentDrawable:window->metalDrawable];
		[cmdBuffer->commandBuffer commit];
		[cmdBuffer->commandBuffer waitUntilCompleted];

		[renderPassDescriptor release];
	}

	void MetalAPI::Draw() {
		@autoreleasepool{
			window->metalDrawable = [window->metalLayer nextDrawable];
			RecordCommandBuffer();
		}
	}


}