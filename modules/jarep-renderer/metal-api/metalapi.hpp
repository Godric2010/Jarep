//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALAPI_HPP
#define JAREP_METALAPI_HPP

#if defined(__APPLE__)

//#define NS_PRIVATE_IMPLEMENTATION
//#define MTL_PRIVATE_IMPLEMENTATION
//#define MTK_PRIVATE_IMPLEMENTATION
//#define CA_PRIVATE_IMPLEMENTATION

#include "IRenderer.hpp"
#include <Metal/Metal.hpp>
#include <Foundation/Foundation.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

//#include <simd/simd.h>

namespace Graphics::Metal {

	class MetalAPI : public IRenderer {
		public:
			MetalAPI();

			~MetalAPI() override;

			void CreateDevice() override;

			void CreateSurface(NativeWindowHandleProvider nativeWindowHandle) override;

			void CreateVertexBuffer() override;

			void CreateShaders() override;

			void CreateCommandQueue() override;

			void CreateGraphicsPipeline() override;

			void RecordCommandBuffer() override;

			void Draw() override;

		private:
			NS::Window* window;
			MTK::View* surface;
			CA::MetalLayer* metalLayer;
			MTL::Device *device;
			MTL::CommandQueue *commandQueue;
			MTL::RenderPipelineState *renderPipelineState;
			MTL::Buffer *vertexPositionBuffer;
			MTL::Buffer *vertexColorBuffer;
	};

}
#endif
#endif //JAREP_METALAPI_HPP
