//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANRENDERPASSBUILDER_HPP
#define JAREP_VULKANRENDERPASSBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanRenderPassFramebuffers.hpp"
#include "VulkanCommandQueue.hpp"
#include "VulkanDataTypeMaps.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>
#include <optional>
#include <functional>

namespace Graphics::Vulkan {
	class VulkanRenderPassBuilder final : public JarRenderPassBuilder {
		public :
			VulkanRenderPassBuilder(
					std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCb) : createCmdQueueCallback(
					createCmdQueueCb) {};

			~VulkanRenderPassBuilder() override;

			VulkanRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) override;

			VulkanRenderPassBuilder* AddDepthStencilAttachment(DepthAttachment depthStencilAttachment) override;

			VulkanRenderPassBuilder* SetMultisamplingCount(uint8_t multisamplingCount) override;

			std::shared_ptr<JarRenderPass>
			Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) override;

		private:
			std::optional<VkAttachmentDescription> m_colorAttachment;
			std::optional<VkAttachmentReference> m_colorAttachmentRef;
			std::optional<VkAttachmentDescription> m_depthStencilAttachment;
			std::optional<VkAttachmentReference> m_depthStencilAttachmentRef;
			std::optional<VkSampleCountFlagBits> m_multisamplingCount;
			std::optional<VkFormat> m_depthFormat;
			std::function<std::shared_ptr<VulkanCommandQueue>()> createCmdQueueCallback;
	};
}


#endif //JAREP_VULKANRENDERPASSBUILDER_HPP
