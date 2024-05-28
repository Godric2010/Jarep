//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALDESCRIPTORCONTENT_HPP
#define JAREP_METALDESCRIPTORCONTENT_HPP

#include <Metal/Metal.hpp>
#include "MetalBuffer.hpp"
#include "MetalImage.hpp"
#include "IRenderAPI.hpp"


namespace Graphics::Metal{

	class MetalDescriptorContent {
		public:
			MetalDescriptorContent() = default;

			virtual ~MetalDescriptorContent() = default;

			virtual void Release() = 0;

			virtual void BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding, StageFlags stageFlags) = 0;

	};

	class MetalUniformDescriptorContent : public MetalDescriptorContent {
		public:
			MetalUniformDescriptorContent(std::vector<std::shared_ptr<MetalBuffer>> buffers) : m_uniformBuffers(std::move(buffers)) {};

			~MetalUniformDescriptorContent() override;

			void Release() override;

			void BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding, StageFlags stageFlags) override;

		private:
			std::vector<std::shared_ptr<MetalBuffer>> m_uniformBuffers;
			uint8_t m_currentBufferIndex = 0;
	};

	class MetalImageDescriptorContent : public MetalDescriptorContent {
		public:
			MetalImageDescriptorContent(std::shared_ptr<MetalImage> image)
					: m_image(image) {};

			~MetalImageDescriptorContent() override;

			void Release() override;

			void BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding, StageFlags stageFlags) override;

		private:
			std::shared_ptr<MetalImage> m_image;
	};
}


#endif //JAREP_METALDESCRIPTORCONTENT_HPP
