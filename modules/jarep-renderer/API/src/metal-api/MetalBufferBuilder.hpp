//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALBUFFERBUILDER_HPP
#define JAREP_METALBUFFERBUILDER_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalBuffer.hpp"
#include "MetalDevice.hpp"

namespace Graphics::Metal {

	class MetalBufferBuilder final : public JarBufferBuilder {
		public:
			MetalBufferBuilder() = default;

			~MetalBufferBuilder() override;

			MetalBufferBuilder* SetUsageFlags(BufferUsage usageFlags) override;

			MetalBufferBuilder* SetMemoryProperties(MemoryProperties memProps) override;

			MetalBufferBuilder* SetBufferData(const void* data, size_t bufferSize) override;

			std::shared_ptr<JarBuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<BufferUsage> m_bufferUsage;
			std::optional<MemoryProperties> m_memoryProperties;
			std::optional<const void*> m_data;
			size_t m_bufferSize;

			static MTL::ResourceUsage bufferUsageToMetal(BufferUsage usage);

			static MTL::ResourceOptions memoryPropertiesToMetal(MemoryProperties memProps);
	};
}

#endif //JAREP_METALBUFFERBUILDER_HPP
