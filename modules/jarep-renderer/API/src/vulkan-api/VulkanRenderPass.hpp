//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANRENDERPASS_HPP
#define JAREP_VULKANRENDERPASS_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPassFramebuffers.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanSurface.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>

namespace Graphics::Vulkan {
	class VulkanFramebuffer;

	class VulkanRenderPassFramebuffers;

	class VulkanSurface;

	class VulkanDevice;

	class VulkanRenderPass final : public JarRenderPass {
		public:
			VulkanRenderPass(std::shared_ptr<VulkanDevice>& device, VkRenderPass renderPass,
			                 std::unique_ptr<VulkanRenderPassFramebuffers>& framebuffers) : m_device(device),
			                                                                                m_renderPass(
					                                                                                renderPass),
			                                                                                m_framebuffers(std::move(
					                                                                                framebuffers)) {};

			~VulkanRenderPass() override;

			void Release() override;

			[[nodiscard]] VkRenderPass getRenderPass() const { return m_renderPass; }

			void RecreateRenderPassFramebuffers(uint32_t width, uint32_t height,
			                                    std::shared_ptr<JarSurface> surface) override;

			std::shared_ptr<VulkanFramebuffer> AcquireNextFramebuffer(uint32_t frameIndex);

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkRenderPass m_renderPass;
			std::unique_ptr<VulkanRenderPassFramebuffers> m_framebuffers;
	};
}

#endif //JAREP_VULKANRENDERPASS_HPP
