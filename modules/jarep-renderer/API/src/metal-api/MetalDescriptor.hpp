//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALDESCRIPTOR_HPP
#define JAREP_METALDESCRIPTOR_HPP

#include "IRenderAPI.hpp"
#include "MetalDescriptorContent.hpp"
#include "MetalDescriptorLayout.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {

	class MetalDescriptor final : public JarDescriptor {
		public:
			MetalDescriptor(uint32_t binding, StageFlags stageFlags, std::shared_ptr<MetalDescriptorContent> content,
			                std::shared_ptr<MetalDescriptorLayout> layout) :
					m_binding(binding), m_stageFlags(stageFlags), m_content(std::move(content)),
					m_descriptorLayout(std::move(layout)) {};

			~MetalDescriptor() override;

			void Release() override;

			std::shared_ptr<JarDescriptorLayout> GetDescriptorLayout() override;

			void BindContentToEncoder(MTL::RenderCommandEncoder* encoder);

		private:
			uint32_t m_binding;
			StageFlags m_stageFlags;
			std::shared_ptr<MetalDescriptorContent> m_content;
			std::shared_ptr<MetalDescriptorLayout> m_descriptorLayout;
	};

}
#endif //JAREP_METALDESCRIPTOR_HPP
