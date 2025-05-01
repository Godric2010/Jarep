//
// Created by Godri on 4/22/2025.
//

using namespace JAREP::Rendering::Core;

template<typename T>
VulkanUniformBuffer<T>::VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice) {
	m_buffer = std::make_unique<VulkanBuffer>(device, physicalDevice, sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

template<typename T>
VulkanUniformBuffer<T>::~VulkanUniformBuffer() {
	m_buffer.reset();
}

template<typename T>
void VulkanUniformBuffer<T>::Update(const T&data) {
	m_buffer->copyFrom(&data, sizeof(T));
}

template<typename T>
VkBuffer VulkanUniformBuffer<T>::getBuffer() const {
	return m_buffer->getBuffer();
}
