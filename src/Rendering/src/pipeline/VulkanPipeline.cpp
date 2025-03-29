//
// Created by Godri on 3/29/2025.
//

#include "VulkanPipeline.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace JAREP::Rendering::Pipeline;

static std::vector<char> readFile(const std::string&fileName) {
	std::filesystem::path shaderBase = std::filesystem::current_path() / "Resources"/ "Shaders";
	std::string shaderPath = (shaderBase / fileName).string();

	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Could not open file " + fileName);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

VulkanPipeline::VulkanPipeline(const VulkanPipelineConfig&config) {
	m_device = config.device;
	m_config = config;
	m_pipeline = VK_NULL_HANDLE;
	m_vertexShaderModule = VK_NULL_HANDLE;
	m_fragmentShaderModule = VK_NULL_HANDLE;

	createGraphicsPipeline();
}

VulkanPipeline::~VulkanPipeline() {
	if (m_pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(m_device, m_pipeline, nullptr);
	}

	if (m_vertexShaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(m_device, m_vertexShaderModule, nullptr);
	}

	if (m_fragmentShaderModule != VK_NULL_HANDLE) {
		vkDestroyShaderModule(m_device, m_fragmentShaderModule, nullptr);
	}
}

VkPipeline VulkanPipeline::get() const {
	return m_pipeline;
}

void VulkanPipeline::createShaderModule(const std::string&filePath, VkShaderModule* shaderModule) {
	const auto code = readFile(filePath);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_vertexShaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Could not create shaders module from: " + filePath);
	}
}

void VulkanPipeline::createGraphicsPipeline() {
	createShaderModule(m_config.vertexShaderPath, &m_vertexShaderModule);
	createShaderModule(m_config.fragmentShaderPath, &m_fragmentShaderModule);

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = m_vertexShaderModule;
	vertexShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = m_fragmentShaderModule;
	fragmentShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

	//TODO: Replace this dummy code with actual vertex data later
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = m_config.primitiveTopology;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_config.extent.width);
	viewport.height = static_cast<float>(m_config.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.extent = m_config.extent;
	scissor.offset = {0, 0};

	VkPipelineViewportStateCreateInfo viewportStateInfo = {};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports = &viewport;
	viewportStateInfo.scissorCount = 1;
	viewportStateInfo.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
	rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.depthClampEnable = VK_FALSE;
	rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerInfo.polygonMode = m_config.polygonMode;
	rasterizerInfo.cullMode = m_config.cullMode;
	rasterizerInfo.frontFace = m_config.frontFace;
	rasterizerInfo.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
	multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.rasterizationSamples = m_config.samples;

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = m_config.depthTestEnable ? VK_TRUE : VK_FALSE;
	depthStencilInfo.depthWriteEnable = m_config.depthWriteEnable ? VK_TRUE : VK_FALSE;
	depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendingInfo = {};
	colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingInfo.logicOpEnable = VK_FALSE;
	colorBlendingInfo.attachmentCount = 1;
	colorBlendingInfo.pAttachments = &colorBlendAttachment;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterizerInfo;
	pipelineInfo.pMultisampleState = &multisampleInfo;
	pipelineInfo.pDepthStencilState = m_config.depthFormat.has_value() ? &depthStencilInfo : nullptr;
	pipelineInfo.pColorBlendState = &colorBlendingInfo;
	pipelineInfo.layout = m_config.pipelineLayout;
	pipelineInfo.renderPass = m_config.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}
