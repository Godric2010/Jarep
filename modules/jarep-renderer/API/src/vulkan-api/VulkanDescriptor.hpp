//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANDESCRIPTOR_HPP
#define JAREP_VULKANDESCRIPTOR_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanDescriptorLayout.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanDescriptorLayout;

	class VulkanDevice;

	class VulkanDescriptor final : public JarDescriptor {
		public:
			VulkanDescriptor(std::shared_ptr<VulkanDevice>& device, VkDescriptorPool descriptorPool,
			                 std::shared_ptr<VulkanDescriptorLayout> layoutBindings,
			                 std::vector<VkDescriptorSet> descriptorSets) : m_device(device),
			                                                                m_descriptorPool(descriptorPool),
			                                                                m_descriptorSetLayout(
					                                                                std::move(layoutBindings)),
			                                                                m_descriptorSets(
					                                                                std::move(descriptorSets)) {}

			~VulkanDescriptor() override;

			void Release() override;

			std::shared_ptr<JarDescriptorLayout> GetDescriptorLayout() override;

			VkDescriptorSet GetNextDescriptorSet();

		private:
			std::shared_ptr<VulkanDevice> m_device;
			std::shared_ptr<VulkanDescriptorLayout> m_descriptorSetLayout;
			VkDescriptorPool m_descriptorPool;
			std::vector<VkDescriptorSet> m_descriptorSets;
			uint8_t m_descriptorSetIndex = 0;
	};
}
#endif //JAREP_VULKANDESCRIPTOR_HPP
