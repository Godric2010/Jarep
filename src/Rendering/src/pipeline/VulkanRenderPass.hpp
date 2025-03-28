//
// Created by Godri on 3/28/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>

namespace JAREP::Rendering::Pipeline {
	struct RenderPassConfig {
		VkFormat colorFormat;
		std::optional<VkFormat> depthFormat = std::nullopt;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
	};

	/**
	 * @class VulkanRenderPass
	 * @brief Encapsulates the creation and configuration of a Vulkan render pass object.
	 *
	 * The VulkanRenderPass class defines how framebuffers are used during rendering,
	 * including which attachments are involves, their formats, load/store operations,
	 * and layout transitions. This class currently supports a basic single-subpass
	 * configuration and is designed to be extendable for depth, multisampling, and
	 * multiple attachments in the future.
	 */
	class VulkanRenderPass {
		public:
			explicit VulkanRenderPass(VkDevice device, const RenderPassConfig&config);

			~VulkanRenderPass();

			VulkanRenderPass(const VulkanRenderPass&) = delete;

			VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

			VkRenderPass get() const;

		private:
			void createRenderPass();

			VkDevice m_device;
			RenderPassConfig m_config;
			VkRenderPass m_renderPass;
	};
}
