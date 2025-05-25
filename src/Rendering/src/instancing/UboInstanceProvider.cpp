//
// Created by Godri on 5/18/2025.
//

#include "UboInstanceProvider.hpp"

using namespace JAREP::Rendering::Instancing;

UBOInstanceProvider::UBOInstanceProvider(const VkDevice device, const VkPhysicalDevice physicalDevice,
                                         const size_t maxInstances) {
	m_buffer = std::make_unique<VulkanUniformBuffer<InstanceData>>(device, physicalDevice,
	                                                               maxInstances * sizeof(InstanceData));
	m_setIndex = 1;

	VulkanDescriptorSetLayout::BindingInfo instBinding = {};
	instBinding.binding = 0;
	instBinding.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	instBinding.count = 1;
	instBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	m_descriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>(device, std::vector{instBinding});

	VulkanDescriptorPool::PoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.count = 1;
	m_descriptorPool = std::make_unique<VulkanDescriptorPool>(device, std::vector{poolSize}, 1);

	m_descriptorSet = std::make_unique<VulkanDescriptorSet>(device, m_descriptorPool->get(),
	                                                        m_descriptorSetLayout->get());

	m_descriptorSet->bindUniformBuffer(0, m_buffer->getBuffer(), maxInstances * sizeof(InstanceData), 0);
}

UBOInstanceProvider::~UBOInstanceProvider() = default;

VkDescriptorSetLayout UBOInstanceProvider::GetDescriptorSetLayout() const {
	return m_descriptorSetLayout->get();
}

void UBOInstanceProvider::PrepareData(VkCommandBuffer&cmd, const std::vector<RenderObject>&objects) {
	std::vector<InstanceData> data(objects.size());
	for (size_t i = 0; i < objects.size(); i++) {
		data[i] = {objects[i].transform};
	}
	m_buffer->Update(data.data(), data.size() * sizeof(InstanceData));
}

void UBOInstanceProvider::BindData(VkCommandBuffer&cmd, const VkPipelineLayout pipelineLayout) {
	const std::vector descriptorSets = {m_descriptorSet->get()};
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, descriptorSets.data(), 0,
	                        nullptr);
}
