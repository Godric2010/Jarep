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
	struct MLibrary;
	struct MCommandQueue;
	struct MCommandBuffer;
	struct MRenderPipeline;
	struct MBuffer;

	class MetalAPI : public IRenderer {
		public:
			MetalAPI();

			~MetalAPI() override;

			void CreateDevice() override;

			void CreateSurface(void *nativeWindowHandle) override;

			void CreateVertexBuffer() override;

			void CreateShaders() override;

			void CreateCommandQueue() override;

			void CreateGraphicsPipeline() override;

			void RecordCommandBuffer() override;

			void Draw() override;

		private:

			MDevice *device;
			MWindow *window;
			MLibrary *library;
			MCommandQueue *cmdQueue;
			MCommandBuffer *cmdBuffer;
			MRenderPipeline *pipeline;
			MBuffer* buffer;


	};

}
#endif //JAREP_METALAPI_HPP
