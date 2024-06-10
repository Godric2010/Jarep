//
// Created by sebastian on 09.06.24.
//

#include "VulkanDescriptorBuilder.hpp"

namespace Graphics::Vulkan {
	VulkanDescriptorBuilder::~VulkanDescriptorBuilder() = default;

	VulkanDescriptorBuilder* VulkanDescriptorBuilder::SetBinding(uint32_t binding) {
		m_binding = std::make_optional<uint32_t>(binding);
		return this;
	}

	VulkanDescriptorBuilder* VulkanDescriptorBuilder::SetStageFlags(StageFlags stageFlags) {
		auto vkStageFlags = convertToVkShaderStageFlagBits(stageFlags);
		m_stageFlags = std::make_optional<VkShaderStageFlagBits>(vkStageFlags);
		return this;
	}

	std::shared_ptr<JarDescriptor>
	VulkanDescriptorBuilder::BuildUniformBufferDescriptor(std::shared_ptr<JarDevice> device,
	                                                      std::vector<std::shared_ptr<JarBuffer>> uniformBuffers) {
		if (!m_stageFlags.has_value())
			throw std::runtime_error("StageFlags not set!");

		if (!m_binding.has_value())
			throw std::runtime_error("Binding not set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		std::vector<std::shared_ptr<VulkanBuffer>> vulkanUniformBuffers;
		for (auto& buffer: uniformBuffers) {
			vulkanUniformBuffers.push_back(reinterpret_cast<std::shared_ptr<VulkanBuffer>&>(buffer));
		}
		auto descriptorAmount = vulkanUniformBuffers.size();

		auto descriptorLayout = BuildDescriptorLayout(vulkanDevice, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		auto descriptorPool = BuildDescriptorPool(vulkanDevice, descriptorAmount, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		auto descriptorSets = AllocateDescriptorSets(vulkanDevice, descriptorPool, descriptorLayout, descriptorAmount);

		for (int i = 0; i < descriptorSets.size(); ++i) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = vulkanUniformBuffers[i]->getBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = vulkanUniformBuffers[i]->getBufferSize();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = m_binding.value();
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
			vkUpdateDescriptorSets(vulkanDevice->getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
		}

		auto vulkanDescriptor = std::make_shared<VulkanDescriptor>(vulkanDevice, descriptorPool, descriptorLayout,
		                                                           descriptorSets);
		return vulkanDescriptor;
	}

	std::shared_ptr<JarDescriptor> VulkanDescriptorBuilder::BuildImageBufferDescriptor(
			std::shared_ptr<JarDevice> device, std::shared_ptr<JarImage> image) {
		if (!m_stageFlags.has_value())
			throw std::runtime_error("StageFlags not set!");

		if (!m_binding.has_value())
			throw std::runtime_error("Binding not set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		auto vulkanImage = reinterpret_cast<std::shared_ptr<VulkanImage>&>(image);
		uint32_t descriptorAmount = 1;

		auto descriptorLayout = BuildDescriptorLayout(vulkanDevice, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		auto descriptorPool = BuildDescriptorPool(vulkanDevice, descriptorAmount,
		                                          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		auto descriptorSets = AllocateDescriptorSets(vulkanDevice, descriptorPool, descriptorLayout, descriptorAmount);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = vulkanImage->getImageView();
		imageInfo.sampler = vulkanImage->getSampler();

		VkWriteDescriptorSet descriptorWriteSampler{};
		descriptorWriteSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWriteSampler.dstSet = descriptorSets[0];
		descriptorWriteSampler.dstBinding = m_binding.value();
		descriptorWriteSampler.dstArrayElement = 0;
		descriptorWriteSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWriteSampler.descriptorCount = 1;
		descriptorWriteSampler.pBufferInfo = nullptr;
		descriptorWriteSampler.pImageInfo = &imageInfo;
		descriptorWriteSampler.pTexelBufferView = nullptr;


		vkUpdateDescriptorSets(vulkanDevice->getLogicalDevice(), 1, &descriptorWriteSampler, 0, nullptr);

		auto vulkanDescriptor = std::make_shared<VulkanDescriptor>(vulkanDevice, descriptorPool, descriptorLayout,
		                                                           descriptorSets);
		return vulkanDescriptor;
	}


	std::shared_ptr<VulkanDescriptorLayout>
	VulkanDescriptorBuilder::BuildDescriptorLayout(std::shared_ptr<VulkanDevice> vulkanDevice,
	                                               VkDescriptorType descriptorType) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = m_binding.value();
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = m_stageFlags.value();
		layoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = 1;
		layoutCreateInfo.pBindings = &layoutBinding;

		VkDescriptorSetLayout descriptorSetLayout;
		if (vkCreateDescriptorSetLayout(vulkanDevice->getLogicalDevice(), &layoutCreateInfo, nullptr,
		                                &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderStepDescriptor set layout!");
		}
		auto vulkanDescriptorLayout = std::make_shared<VulkanDescriptorLayout>(vulkanDevice, descriptorSetLayout,
		                                                                       m_binding.value());
		return vulkanDescriptorLayout;
	}

	VkDescriptorPool VulkanDescriptorBuilder::BuildDescriptorPool(std::shared_ptr<VulkanDevice> vulkanDevice,
	                                                              uint32_t descriptorSetCount,
	                                                              VkDescriptorType descriptorType) {
		VkDescriptorPoolSize poolSize{};
		poolSize.type = descriptorType;
		poolSize.descriptorCount = descriptorSetCount;

		VkDescriptorPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCreateInfo.poolSizeCount = 1;
		poolCreateInfo.pPoolSizes = &poolSize;
		poolCreateInfo.maxSets = descriptorSetCount;

		VkDescriptorPool descriptorPool;
		if (vkCreateDescriptorPool(vulkanDevice->getLogicalDevice(), &poolCreateInfo, nullptr, &descriptorPool) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderStepDescriptor pool!");
		}
		return descriptorPool;
	}

	std::vector<VkDescriptorSet>
	VulkanDescriptorBuilder::AllocateDescriptorSets(std::shared_ptr<VulkanDevice> vulkanDevice,
	                                                VkDescriptorPool descriptorPool,
	                                                std::shared_ptr<VulkanDescriptorLayout> descriptorLayout,
	                                                uint32_t descriptorSetCount) {
		std::vector<VkDescriptorSetLayout> layouts(descriptorSetCount, descriptorLayout->getDescriptorSetLayout());
		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = descriptorPool;
		allocateInfo.descriptorSetCount = descriptorSetCount;
		allocateInfo.pSetLayouts = layouts.data();

		std::vector<VkDescriptorSet> descriptorSets(descriptorSetCount);
		if (vkAllocateDescriptorSets(vulkanDevice->getLogicalDevice(), &allocateInfo, descriptorSets.data()) !=
		    VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderStepDescriptor sets!");
		}
		return descriptorSets;
	}

	VkShaderStageFlagBits
	VulkanDescriptorBuilder::convertToVkShaderStageFlagBits(Graphics::StageFlags stageFlags) {
		int flagBits = 0;
		auto maskValue = static_cast<std::underlying_type<StageFlags>::type>(stageFlags);
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::VertexShader))
			flagBits |= VK_SHADER_STAGE_VERTEX_BIT;
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::FragmentShader))
			flagBits |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::GeometryShader))
			flagBits |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if (maskValue & static_cast<std::underlying_type<StageFlags>::type>(StageFlags::ComputeShader))
			flagBits |= VK_SHADER_STAGE_COMPUTE_BIT;
		return static_cast<VkShaderStageFlagBits>(flagBits);

	}
}