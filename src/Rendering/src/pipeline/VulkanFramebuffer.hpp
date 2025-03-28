//
// Created by Godri on 3/28/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

namespace JAREP::Rendering::Pipeline {
	/**
	 * @class VulkanFramebuffer
	 * @brief Encapsulates the creation and destruction of a Vulkan framebuffer.
	 *
	 * The VulkanFramebuffer class binds together the attachments for a render pass,
	 * typically including color and optional depth images from the swapchain. It is created
	 * for each image view that will be rendered into and is linkes to a specific render pass.
	 */
	class VulkanFramebuffer {
		public:
			VulkanFramebuffer(VkDevice device, VkRenderPass renderPass, VkExtent2D extent,
			                  const std::vector<VkImageView>&attachments);

			~VulkanFramebuffer();

			VulkanFramebuffer(const VulkanFramebuffer&) = delete;

			VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

			VkFramebuffer get() const;

		private:
			void createFramebuffer();

			VkDevice m_device;
			VkRenderPass m_renderPass;
			VkExtent2D m_extent;
			std::vector<VkImageView> m_attachments;
			VkFramebuffer m_framebuffer;
	};
}
