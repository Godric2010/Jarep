//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALAPI_HPP
#define JAREP_METALAPI_HPP

#include "IRenderer.hpp"
namespace Graphics::Metal {

	/// Pimpl of the metalAPI class to avoid objective-C++ code in a C++ class
	struct MDevice;
	struct MWindow;
	struct MetalImpl;

	class MetalAPI : public IRenderer {
		public:
			MetalAPI();

			~MetalAPI() override;

			void CreateDevice() override;
			void CreateSurface(void* nativeWindowHandle) override;
			void CreateCommandQueue();
			void LoadShaders();
			void CreatePipeline();
			void CreateVertexBuffer();
			void CreateRenderPassDescriptor();
			void CreateCommandBuffer();
			void EncodeRenderingCommands();
			void SubmitCommandBufferToQueue();
			void PresentImage();

		private:

			MDevice* device;
			MWindow* window;
	};

}
#endif //JAREP_METALAPI_HPP
