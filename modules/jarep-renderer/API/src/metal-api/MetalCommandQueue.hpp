//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALCOMMANDQUEUE_HPP
#define JAREP_METALCOMMANDQUEUE_HPP

#include "IRenderAPI.hpp"
#include "MetalCommandBuffer.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {

	class MetalCommandQueue final : public JarCommandQueue {
		public:
			MetalCommandQueue(MTL::CommandQueue* cmdQueue) : queue(cmdQueue) {
			}

			~MetalCommandQueue() override;

			JarCommandBuffer* getNextCommandBuffer() override;

			void Release() override;

		private:
			MTL::CommandQueue* queue;
	};


}

#endif //JAREP_METALCOMMANDQUEUE_HPP
