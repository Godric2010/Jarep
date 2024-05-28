//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalDescriptor.hpp"

namespace Graphics::Metal {

	MetalDescriptor::~MetalDescriptor() = default;

	void MetalDescriptor::Release() {}

	std::shared_ptr<JarDescriptorLayout> MetalDescriptor::GetDescriptorLayout() { return m_descriptorLayout; }

	void MetalDescriptor::BindContentToEncoder(MTL::RenderCommandEncoder* encoder) {
		m_content->BindContentToEncoder(encoder, m_binding, m_stageFlags);
	}
}