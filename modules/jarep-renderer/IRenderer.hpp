//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

#include <iostream>

#include "NativeWindowHandleProvider.hpp"

namespace Graphics {

	class IRenderer {
		public:
			virtual ~IRenderer() = default;

			virtual void RegisterPhysicalDevice() = 0;

			virtual void CreateLogicalDevice() = 0;

			virtual void CreateSurface(NativeWindowHandleProvider *windowHandleProvider) = 0;

			virtual void CreateVertexBuffer() = 0;

			virtual void CreateShaders() = 0;

			virtual void CreateCommandQueue() = 0;

			virtual void CreateGraphicsPipeline() = 0;

			virtual void RecordCommandBuffer() = 0;

			virtual void Draw() = 0;

			virtual void Shutdown() = 0;

	};

}


#endif //JAREP_IRENDERER_HPP
