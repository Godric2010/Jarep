//
// Created by sebastian on 09.06.24.
//

#include "VulkanGraphicsPipelineBuilder.hpp"

namespace Graphics::Vulkan {


	VulkanGraphicsPipelineBuilder::VulkanGraphicsPipelineBuilder() = default;

	VulkanGraphicsPipelineBuilder::~VulkanGraphicsPipelineBuilder() = default;

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::SetShaderStage(Graphics::ShaderStage shaderStage) {

		auto vulkanVertexShaderModule = reinterpret_cast<std::shared_ptr<VulkanShaderModule>&>(shaderStage.vertexShaderModule);
		auto vulkanFragmentShaderModule = reinterpret_cast<std::shared_ptr<VulkanShaderModule>&>(shaderStage.fragmentShaderModule);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vulkanVertexShaderModule->getShaderModule();
		vertShaderStageInfo.pName = shaderStage.mainFunctionName.c_str();

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = vulkanFragmentShaderModule->getShaderModule();
		fragShaderStageInfo.pName = shaderStage.mainFunctionName.c_str();

		m_shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();
		m_shaderStages.push_back(vertShaderStageInfo);
		m_shaderStages.push_back(fragShaderStageInfo);
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = reinterpret_cast<std::shared_ptr<VulkanRenderPass>&>(renderPass);
		return this;
	}

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::SetVertexInput(Graphics::VertexInput vertexInput) {

		m_bindingDescriptions = std::vector<VkVertexInputBindingDescription>();
		for (auto bindingDescData: vertexInput.bindingDescriptions) {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = bindingDescData.bindingIndex;
			bindingDescription.stride = bindingDescData.stride;
			bindingDescription.inputRate = inputRateMap[bindingDescData.inputRate];
			m_bindingDescriptions.push_back(bindingDescription);
		}
		m_attributeDescriptions = std::vector<VkVertexInputAttributeDescription>();
		for (auto attributeDescData: vertexInput.attributeDescriptions) {
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.binding = attributeDescription.binding;
			attributeDescription.location = attributeDescData.attributeLocation;
			attributeDescription.format = vertexFormatMap[attributeDescData.vertexFormat];
			attributeDescription.offset = attributeDescData.offset;
			m_attributeDescriptions.push_back(attributeDescription);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = m_bindingDescriptions.size();
		vertexInputInfo.pVertexBindingDescriptions = m_bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = m_attributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();

		m_vertexInput = std::make_optional(vertexInputInfo);
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetInputAssemblyTopology(Graphics::InputAssemblyTopology topology) {
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = topologyMap[topology];
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		m_inputAssembly = std::make_optional(inputAssembly);

		return this;
	}

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::SetMultisamplingCount(uint16_t multisamplingCount) {

		VkSampleCountFlagBits sampleCountFlagBits = convertToVkSampleCountFlagBits(multisamplingCount);

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = sampleCountFlagBits;
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.minSampleShading = 0.2f;

		m_multisampling = std::make_optional(multisampling);
		return this;
	}

	VulkanGraphicsPipelineBuilder* VulkanGraphicsPipelineBuilder::BindDescriptorLayouts(
			std::vector<std::shared_ptr<JarDescriptorLayout>> descriptorLayouts) {

		for (auto& descriptorLayout: descriptorLayouts) {
			auto vulkanDescriptorLayout = reinterpret_cast<std::shared_ptr<VulkanDescriptorLayout>&>(descriptorLayout);
			m_descriptorSetLayouts.push_back(vulkanDescriptorLayout->getDescriptorSetLayout());
		}
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetColorBlendAttachments(Graphics::ColorBlendAttachment colorBlendAttachment) {
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.colorBlendOp = blendOpMap[colorBlendAttachment.rgbBlendOperation];
		colorBlendAttachmentState.alphaBlendOp = blendOpMap[colorBlendAttachment.alphaBlendOperation];
		colorBlendAttachmentState.blendEnable = static_cast<VkBool32>(colorBlendAttachment.blendingEnabled);
		colorBlendAttachmentState.srcColorBlendFactor = blendFactorMap[colorBlendAttachment.sourceRgbBlendFactor];
		colorBlendAttachmentState.srcAlphaBlendFactor = blendFactorMap[colorBlendAttachment.sourceAlphaBlendFactor];
		colorBlendAttachmentState.dstColorBlendFactor = blendFactorMap[colorBlendAttachment.destinationRgbBlendFactor];
		colorBlendAttachmentState.dstAlphaBlendFactor = blendFactorMap[colorBlendAttachment.destinationAlphaBlendFactor];
		colorBlendAttachmentState.colorWriteMask = convertToColorComponentFlagBits(colorBlendAttachment.colorWriteMask);

		m_colorBlendAttachmentStates = std::vector<VkPipelineColorBlendAttachmentState>();
		m_colorBlendAttachmentStates.push_back(colorBlendAttachmentState);

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = m_colorBlendAttachmentStates.size();
		colorBlending.pAttachments = m_colorBlendAttachmentStates.data();

		m_colorBlend = std::make_optional(colorBlending);
		return this;
	}

	VulkanGraphicsPipelineBuilder*
	VulkanGraphicsPipelineBuilder::SetDepthStencilState(Graphics::DepthStencilState depthStencilState) {

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.depthTestEnable = static_cast<VkBool32>(depthStencilState.depthTestEnable);
		depthStencilStateCreateInfo.depthCompareOp = compareOpMap[depthStencilState.depthCompareOp];
		depthStencilStateCreateInfo.depthWriteEnable = static_cast<VkBool32>(depthStencilState.depthWriteEnable);
		depthStencilStateCreateInfo.stencilTestEnable = static_cast<VkBool32>(depthStencilState.stencilTestEnable);
		if (depthStencilState.stencilTestEnable) {
			VkStencilOpState stencilOpState{};
			stencilOpState.failOp = stencilCompareOpMap[depthStencilState.stencilFailOp];
			stencilOpState.passOp = stencilCompareOpMap[depthStencilState.stencilPassOp];
			stencilOpState.depthFailOp = stencilCompareOpMap[depthStencilState.stencilDepthFailOp];
			stencilOpState.compareOp = compareOpMap[depthStencilState.stencilCompareOp];
			stencilOpState.compareMask = 0xFF;
			stencilOpState.writeMask = 0xFF;
			stencilOpState.reference = 1;

			depthStencilStateCreateInfo.front = stencilOpState;
			depthStencilStateCreateInfo.back = stencilOpState;
		}
		m_depthStencil = std::make_optional(depthStencilStateCreateInfo);
		return this;
	}

	std::shared_ptr<JarPipeline> VulkanGraphicsPipelineBuilder::Build(std::shared_ptr<JarDevice> device) {

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
		pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(vulkanDevice->getLogicalDevice(), &pipelineLayoutCreateInfo, nullptr,
		                           &m_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
		std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
		                                             VK_DYNAMIC_STATE_DEPTH_BIAS};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_TRUE;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = m_shaderStages.size();
		pipelineInfo.pStages = m_shaderStages.data();
		pipelineInfo.pVertexInputState = &m_vertexInput.value();
		pipelineInfo.pInputAssemblyState = &m_inputAssembly.value();
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &m_multisampling.value();
		if (m_depthStencil.has_value()) {
			pipelineInfo.pDepthStencilState = &m_depthStencil.value();
		}
		pipelineInfo.pColorBlendState = &m_colorBlend.value();
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass->getRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		auto result = vkCreateGraphicsPipelines(vulkanDevice->getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
		                                        nullptr,
		                                        &m_pipeline);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline");
		}

		return std::make_shared<VulkanGraphicsPipeline>(vulkanDevice, m_pipelineLayout, m_pipeline, m_renderPass);
	}

	VkColorComponentFlags
	VulkanGraphicsPipelineBuilder::convertToColorComponentFlagBits(Graphics::ColorWriteMask colorWriteMask) {
		VkColorComponentFlags flagBits = 0;
		auto maskValue = static_cast<std::underlying_type<ColorWriteMask>::type>(colorWriteMask);
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Red))
			flagBits |= VK_COLOR_COMPONENT_R_BIT;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Green))
			flagBits |= VK_COLOR_COMPONENT_G_BIT;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Blue))
			flagBits |= VK_COLOR_COMPONENT_B_BIT;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Alpha))
			flagBits |= VK_COLOR_COMPONENT_A_BIT;
		return flagBits;
	}
}