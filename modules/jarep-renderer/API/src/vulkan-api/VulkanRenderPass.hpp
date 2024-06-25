//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANRENDERPASS_HPP
#define JAREP_VULKANRENDERPASS_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanSurface;

	class VulkanDevice;

	class VulkanRenderPass final : public JarRenderPass {
		public:
		VulkanRenderPass(std::shared_ptr<VulkanDevice>& device, VkRenderPass renderPass) : m_device(device),
		                                                                                   m_renderPass(renderPass) {};

		~VulkanRenderPass() override;

		void Release() override;

		[[nodiscard]] VkRenderPass GetRenderPass() const { return m_renderPass; }

		private:
		std::shared_ptr<VulkanDevice> m_device;
		VkRenderPass m_renderPass;
	};
}// namespace Graphics::Vulkan

#endif//JAREP_VULKANRENDERPASS_HPP
