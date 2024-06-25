//
// Created by sebastian on 24.06.24.
//

#ifndef JAREP_VULKANFRAMEBUFFER_HPP
#define JAREP_VULKANFRAMEBUFFER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImageBuffer.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanDevice;
	class VulkanImageBuffer;

	class VulkanFramebuffer final : public JarFramebuffer {

		public:
		VulkanFramebuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanImageBuffer> targetBuffer, VkFramebuffer framebuffer, VkExtent2D framebufferExtent);

		~VulkanFramebuffer() override = default;

		void Release() override;

		inline VkFramebuffer GetFramebuffer() { return m_framebuffer; }

		inline VkExtent2D GetFramebufferExtent() { return m_framebufferExtent; }

		VkImage GetSrcImage() ;

		private:
		std::shared_ptr<VulkanDevice> m_device;
		std::shared_ptr<VulkanImageBuffer> m_targetImageBuffer;
		VkFramebuffer m_framebuffer;
		VkExtent2D m_framebufferExtent;
	};

}// namespace Graphics::Vulkan

#endif//JAREP_VULKANFRAMEBUFFER_HPP
