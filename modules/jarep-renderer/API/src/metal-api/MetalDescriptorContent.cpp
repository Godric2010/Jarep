//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalDescriptorContent.hpp"

namespace Graphics::Metal {

	MetalUniformDescriptorContent::~MetalUniformDescriptorContent() = default;

	void MetalUniformDescriptorContent::Release() {}

	void MetalUniformDescriptorContent::BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding,
	                                                         Graphics::StageFlags stageFlags) {
		if (stageFlags != StageFlags::VertexShader)
			std::runtime_error("Uniform buffer can only be bound to the vertex shader!");

		encoder->setVertexBuffer(m_uniformBuffers[m_currentBufferIndex]->getBuffer().value(), 0, binding);

		m_currentBufferIndex = (m_currentBufferIndex + 1) % m_uniformBuffers.size();
	}

	MetalImageDescriptorContent::~MetalImageDescriptorContent() = default;

	void MetalImageDescriptorContent::Release() {}

	void MetalImageDescriptorContent::BindContentToEncoder(MTL::RenderCommandEncoder* encoder, uint32_t binding,
	                                                       Graphics::StageFlags stageFlags) {
		if (stageFlags != StageFlags::FragmentShader)
			std::runtime_error("Image buffer can only be bound to the fragment shader!");

		encoder->setFragmentTexture(m_image->getTexture(), binding);
		encoder->setFragmentSamplerState(m_image->getSampler(), binding);

	}
}