//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANDESCRIPTORBUILDER_HPP
#define JAREP_VULKANDESCRIPTORBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanDescriptorLayout.hpp"
#include "VulkanDescriptor.hpp"
#include "VulkanBuffer.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>

namespace Graphics::Vulkan {
	class VulkanDescriptorLayout;

	class VulkanDescriptor;

	class VulkanDevice;

	class VulkanDescriptorBuilder : public JarDescriptorBuilder {
		public:
			VulkanDescriptorBuilder() = default;

			~VulkanDescriptorBuilder() override;

			VulkanDescriptorBuilder* SetBinding(uint32_t binding) override;

			VulkanDescriptorBuilder* SetStageFlags(StageFlags stageFlags) override;


			std::shared_ptr<JarDescriptor>
			BuildUniformBufferDescriptor(std::shared_ptr<JarDevice> device,
			                             std::vector<std::shared_ptr<JarBuffer>> uniformBuffers) override;

			std::shared_ptr<JarDescriptor>
			BuildImageBufferDescriptor(std::shared_ptr<JarDevice> device, std::shared_ptr<JarImage> image) override;

		private:
			std::optional<uint32_t> m_binding;
			std::optional<VkShaderStageFlagBits> m_stageFlags;

			static VkShaderStageFlagBits convertToVkShaderStageFlagBits(StageFlags stageFlags);

			std::shared_ptr<VulkanDescriptorLayout>
			BuildDescriptorLayout(std::shared_ptr<VulkanDevice> vulkanDevice, VkDescriptorType descriptorType);

			VkDescriptorPool
			BuildDescriptorPool(std::shared_ptr<VulkanDevice> vulkanDevice, uint32_t descriptorSetCount,
			                    VkDescriptorType descriptorType);

			std::vector<VkDescriptorSet>
			AllocateDescriptorSets(std::shared_ptr<VulkanDevice> vulkanDevice, VkDescriptorPool descriptorPool,
			                       std::shared_ptr<VulkanDescriptorLayout> descriptorLayout,
			                       uint32_t descriptorSetCount);
	};
}
#endif //JAREP_VULKANDESCRIPTORBUILDER_HPP
