//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

#include <iostream>
namespace Graphics {

	class IRenderer {
		public:
			virtual ~IRenderer() = default;

			virtual void CreateDevice() = 0;
			virtual void CreateSurface(void* nativeWindowHandle) = 0;
			virtual void CreateVertexBuffer() = 0;
			virtual void CreateShaders() = 0;
			virtual void CreateCommandQueue() = 0;
			virtual void CreateGraphicsPipeline() = 0;
			virtual void RecordCommandBuffer() = 0;
			virtual void Draw() = 0;

	};

}


#endif //JAREP_IRENDERER_HPP
