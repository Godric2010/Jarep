//
// Created by Sebastian Borsch on 24.10.23.
//

#include "metalapi.hpp"

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>

namespace Graphics::Metal{

	struct MetalImpl{
		id<MTLDevice> device;
	};


	MetalAPI::MetalAPI() {
		impl = new MetalImpl();
	}

	MetalAPI::~MetalAPI() {
		delete impl;
		impl = nullptr;
	}
	void MetalAPI::CreateDevice() {
		impl->device = MTLCreateSystemDefaultDevice();
		if(!impl->device){
			fprintf(stderr, "No device supporting metal was found");
			abort();
		}
	}

	void MetalAPI::CreateCommandQueue() {

	}

	void MetalAPI::CreateLibrary() {

	}


}