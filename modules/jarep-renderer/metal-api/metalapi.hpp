//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALAPI_HPP
#define JAREP_METALAPI_HPP

#if defined(__APPLE__)

#include "IRenderer.hpp"
#include <Metal/Metal.hpp>
#include <Foundation/Foundation.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>


namespace Graphics::Metal {

	class MetalAPI : public IRenderer {
		public:
			MetalAPI();

			~MetalAPI() override;

			/// Create a logical device for metal means to link the MTLdevice with the surface and push this into the window settings.
			void CreateLogicalDevice() override;

			/// Create an MTLDevice and provide it for future usage
			void RegisterPhysicalDevice() override;

			/// Create a render surface but don't push it to the window, since the creation of the MTL Device occurs later
			void CreateSurface(NativeWindowHandleProvider *nativeWindowHandle) override;

			void CreateVertexBuffer() override;

			void CreateShaders() override;

			void CreateCommandQueue() override;

			void CreateGraphicsPipeline() override;

			void RecordCommandBuffer() override;

			void Draw() override;

			void Shutdown() override;

		private:
			NS::Window *window;
			MTK::View *surface;
			CA::MetalLayer *metalLayer;
			MTL::Device *device;
			MTL::CommandQueue *commandQueue;
			MTL::RenderPipelineState *renderPipelineState;
			MTL::Buffer *vertexPositionBuffer;
			MTL::Buffer *vertexColorBuffer;
	};

}
#endif
#endif //JAREP_METALAPI_HPP
