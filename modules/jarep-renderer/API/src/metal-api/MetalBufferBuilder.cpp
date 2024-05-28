//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalBufferBuilder.hpp"

namespace Graphics::Metal{

	MetalBufferBuilder::~MetalBufferBuilder() =
	default;

	MetalBufferBuilder* MetalBufferBuilder::SetUsageFlags(BufferUsage usageFlags) {
		m_bufferUsage = std::make_optional(usageFlags);
		return this;
	}

	MetalBufferBuilder* MetalBufferBuilder::SetMemoryProperties(MemoryProperties memProps) {
		m_memoryProperties = std::make_optional(memProps);
		return this;
	}

	MetalBufferBuilder* MetalBufferBuilder::SetBufferData(const void* data, size_t bufferSize) {
		m_bufferSize = bufferSize;
		m_data = std::make_optional(data);
		return this;
	}

	std::shared_ptr<JarBuffer> MetalBufferBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		if (m_bufferSize <= 0 || !m_data.has_value() || !m_memoryProperties.has_value() ||
		    !m_bufferUsage.has_value())
			throw std::runtime_error("Could not create buffer! Provided data is insufficient.");

		const auto bufferOptions = bufferUsageToMetal(m_bufferUsage.value()) & memoryPropertiesToMetal(
				m_memoryProperties.value());
		MTL::Buffer* buffer = metalDevice->getDevice()->newBuffer(m_bufferSize, bufferOptions);
		memcpy(buffer->contents(), m_data.value(), m_bufferSize);
		buffer->didModifyRange(NS::Range::Make(0, buffer->length()));

		return std::make_shared<MetalBuffer>(buffer);
	}

	MTL::ResourceOptions MetalBufferBuilder::bufferUsageToMetal(const BufferUsage bufferUsage) {
		switch (bufferUsage) {
			case BufferUsage::VertexBuffer:
			case BufferUsage::IndexBuffer:
			case BufferUsage::UniformBuffer:
				return MTL::ResourceUsageRead;
			case BufferUsage::StoreBuffer:
				return MTL::ResourceUsageRead & MTL::ResourceUsageWrite;
			case BufferUsage::TransferSrc:
				return MTL::ResourceUsageRead & MTL::ResourceUsageWrite;
			case BufferUsage::TransferDest:
				return MTL::ResourceUsageRead & MTL::ResourceUsageWrite;
			default:;
		}
		return 0;
	}

	MTL::ResourceOptions MetalBufferBuilder::memoryPropertiesToMetal(const MemoryProperties memProps) {

		if (memProps & MemoryProperties::DeviceLocal)
			return MTL::StorageModePrivate;

		if (memProps & MemoryProperties::HostVisible)
			return MTL::StorageModeShared;

		if (memProps & MemoryProperties::HostCoherent)
			return MTL::StorageModeManaged;

		if (memProps & MemoryProperties::HostCached)
			return MTL::StorageModeManaged;

		if (memProps & MemoryProperties::LazilyAllocation)
			return MTL::StorageModeManaged;

		return 0;
	}
}