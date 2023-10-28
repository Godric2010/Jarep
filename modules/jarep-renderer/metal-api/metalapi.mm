//
// Created by Sebastian Borsch on 24.10.23.
//

#include "metalapi.hpp"

#include <Metal/Metal.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/QuartzCore.h>

namespace Graphics::Metal {

	struct MDevice {
		id <MTLDevice> device;
	};

	struct MWindow {
		NSWindow *metalWindow;
		CAMetalLayer *metalLayer;
	};

	struct MetalImpl {
		id <MTLDevice> device;
		id <MTLCommandQueue> commandQueue;
		id <MTLLibrary> library;
		id <MTLRenderPipelineState> pipelineState;
		id <MTLBuffer> vertexBuffer;
		MTLRenderPassDescriptor *renderPassDesc;
		id <MTLCommandBuffer> commandBuffer;
	};


	MetalAPI::MetalAPI() {
		device = new MDevice();
//		window = new MWindow();
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





}