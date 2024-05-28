//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALBUFFER_HPP
#define JAREP_METALBUFFER_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {

	class MetalBuffer final : public JarBuffer {
		public:
			explicit MetalBuffer(MTL::Buffer* buffer) : m_buffer(buffer) {
			}

			~MetalBuffer() override;

			void Release() override;

			void Update(const void* data, size_t bufferSize) override;

			[[nodiscard]] std::optional<MTL::Buffer*> getBuffer();

		private:
			MTL::Buffer* m_buffer;
	};
}


#endif //JAREP_METALBUFFER_HPP
