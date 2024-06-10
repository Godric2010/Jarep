//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANDESCRIPTORLAYOUT_HPP
#define JAREP_VULKANDESCRIPTORLAYOUT_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanDescriptorLayout : public JarDescriptorLayout {
		public:
			VulkanDescriptorLayout(std::shared_ptr<VulkanDevice>& device, VkDescriptorSetLayout descriptorSetLayout,
			                       uint32_t layoutBinding)
					: m_device(device), m_descriptorSetLayout(descriptorSetLayout), m_layoutBinding(layoutBinding) {};

			~VulkanDescriptorLayout() override;

			void Release() override;

			[[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

			[[nodiscard]] uint32_t getLayoutBinding() const { return m_layoutBinding; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkDescriptorSetLayout m_descriptorSetLayout;
			uint32_t m_layoutBinding;

	};
}
#endif //JAREP_VULKANDESCRIPTORLAYOUT_HPP
