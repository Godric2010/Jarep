//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalDescriptorBuilder.hpp"

namespace Graphics::Metal {

	MetalDescriptorBuilder::~MetalDescriptorBuilder() = default;

	MetalDescriptorBuilder* MetalDescriptorBuilder::SetBinding(uint32_t binding) {
		m_binding = std::make_optional(binding);
		return this;
	}

	MetalDescriptorBuilder* MetalDescriptorBuilder::SetStageFlags(Graphics::StageFlags stageFlags) {
		m_stageFlags = std::make_optional(stageFlags);
		return this;
	}

	std::shared_ptr<JarDescriptor>
	MetalDescriptorBuilder::BuildUniformBufferDescriptor(std::shared_ptr<JarDevice> device,
	                                                     std::vector<std::shared_ptr<JarBuffer>> uniformBuffers) {
		if (!m_binding.has_value() || !m_stageFlags.has_value())
			throw std::runtime_error("Could not create descriptor! Binding and/or stage flags are undefined!");

		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		std::vector<std::shared_ptr<MetalBuffer>> metalBuffers;
		for (auto buffer: uniformBuffers) {
			auto metalBuffer = reinterpret_cast<std::shared_ptr<MetalBuffer>&>(buffer);
			metalBuffers.push_back(metalBuffer);
		}

		auto content = std::make_shared<MetalUniformDescriptorContent>(metalBuffers);
		auto layout = std::make_shared<MetalDescriptorLayout>();

		return std::make_shared<MetalDescriptor>(m_binding.value(), m_stageFlags.value(), content, layout);
	}

	std::shared_ptr<JarDescriptor> MetalDescriptorBuilder::BuildImageBufferDescriptor(std::shared_ptr<JarDevice> device,
	                                                                                  std::shared_ptr<JarImage> image) {

		if (!m_binding.has_value() || !m_stageFlags.has_value())
			throw std::runtime_error("Could not create descriptor! Binding and/or stage flags are undefined!");

		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		auto metalImage = reinterpret_cast<std::shared_ptr<MetalImage>&>(image);

		auto content = std::make_shared<MetalImageDescriptorContent>(metalImage);
		auto layout = std::make_shared<MetalDescriptorLayout>();

		return std::make_shared<MetalDescriptor>(m_binding.value(), m_stageFlags.value(), content, layout);
	}
}