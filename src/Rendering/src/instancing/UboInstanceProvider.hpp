//
// Created by Godri on 5/18/2025.
//

#pragma once
#include "InstanceDataProvider.hpp"
#include "../core/VulkanDescriptorPool.hpp"
#include "../core/VulkanDescriptorSet.hpp"
#include "../core/VulkanDescriptorSetLayout.hpp"
#include "../core/VulkanUniformBuffer.hpp"

namespace JAREP::Rendering::Instancing {
	class UBOInstanceProvider : public InstanceDataProvider {
		public:
			UBOInstanceProvider(VkDevice device, VkPhysicalDevice physicalDevice, size_t maxInstances);

			~UBOInstanceProvider() override;

			[[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout() const override;

			void PrepareData(VkCommandBuffer&cmd, const std::vector<RenderObject>&objects) override;

			void BindData(VkCommandBuffer&cmd, VkPipelineLayout pipelineLayout) override;

		private:
			std::unique_ptr<VulkanUniformBuffer<InstanceData>> m_buffer;
			std::unique_ptr<VulkanDescriptorSetLayout> m_descriptorSetLayout;
			std::unique_ptr<VulkanDescriptorPool> m_descriptorPool;
			std::unique_ptr<VulkanDescriptorSet> m_descriptorSet;
			uint32_t m_setIndex;
	};
}
