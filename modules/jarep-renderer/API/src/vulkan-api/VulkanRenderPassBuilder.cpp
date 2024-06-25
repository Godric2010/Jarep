//
// Created by sebastian on 09.06.24.
//

#include "VulkanRenderPassBuilder.hpp"

namespace Graphics::Vulkan { ;

	VulkanRenderPassBuilder::~VulkanRenderPassBuilder() = default;

	VulkanRenderPassBuilder* VulkanRenderPassBuilder::AddColorAttachment(Graphics::ColorAttachment colorAttachment) {
		VkAttachmentDescription colorAttachmentDesc{};
		colorAttachmentDesc.format = pixelFormatMap[colorAttachment.imageFormat];
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = loadOpMap[colorAttachment.loadOp];
		colorAttachmentDesc.storeOp = storeOpMap[colorAttachment.storeOp];
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		m_colorAttachment = std::make_optional(colorAttachmentDesc);

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_colorAttachmentRef = std::make_optional(colorAttachmentRef);

		return this;
	}

	VulkanRenderPassBuilder* VulkanRenderPassBuilder::AddDepthStencilAttachment(
			Graphics::DepthAttachment depthStencilAttachment) {
		VkAttachmentDescription depthAttachment{};
		m_depthFormat = std::make_optional(pixelFormatMap[depthStencilAttachment.Format]);
		depthAttachment.format = m_depthFormat.value();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = loadOpMap[depthStencilAttachment.DepthLoadOp];
		depthAttachment.storeOp = storeOpMap[depthStencilAttachment.DepthStoreOp];
		if (depthStencilAttachment.Stencil.has_value()) {
			depthAttachment.stencilLoadOp = loadOpMap[depthStencilAttachment.Stencil.value().StencilLoadOp];
			depthAttachment.stencilStoreOp = storeOpMap[depthStencilAttachment.Stencil.value().StencilStoreOp];
		}
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_depthStencilAttachment = std::make_optional(depthAttachment);

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_depthStencilAttachmentRef = std::make_optional(depthAttachmentRef);
		return this;
	}

	VulkanRenderPassBuilder* VulkanRenderPassBuilder::SetMultisamplingCount(uint8_t multisamplingCount) {
		m_multisamplingCount = std::make_optional(convertToVkSampleCountFlagBits(multisamplingCount));
		return this;
	}

	std::shared_ptr<JarRenderPass>
	VulkanRenderPassBuilder::Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) {


		if (!m_colorAttachment.has_value())
			throw std::runtime_error("Could not create render pass without color attachment set!");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);
		auto vulkanSurface = reinterpret_cast<std::shared_ptr<VulkanSurface>&>(surface);

		if (!m_multisamplingCount.has_value())
			throw std::runtime_error("Could not create render pass without multisampling count set!");

		auto msaaSamples = m_multisamplingCount.value();
		m_colorAttachment.value().samples = msaaSamples;

		std::vector<VkAttachmentDescription> attachments = std::vector<VkAttachmentDescription>();
		attachments.push_back(m_colorAttachment.value());

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &m_colorAttachmentRef.value();

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = m_colorAttachment.value().format;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		if (m_depthStencilAttachment.has_value()) {
			m_depthStencilAttachment.value().samples = msaaSamples;
			attachments.push_back(m_depthStencilAttachment.value());
			subpass.pDepthStencilAttachment = &m_depthStencilAttachmentRef.value();

			dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		attachments.push_back(colorAttachmentResolve);

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = attachments.size() - 1;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass renderPass;
		if (vkCreateRenderPass(vulkanDevice->getLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		auto vulkanRenderPass = std::make_shared<VulkanRenderPass>(vulkanDevice, renderPass);
		return vulkanRenderPass;
	}
}