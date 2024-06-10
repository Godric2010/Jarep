//
// Created by sebastian on 09.06.24.
//

#include "VulkanShaderModuleBuilder.hpp"

namespace Graphics::Vulkan {
	VulkanShaderModuleBuilder::~VulkanShaderModuleBuilder() = default;

	VulkanShaderModuleBuilder* VulkanShaderModuleBuilder::SetShader(std::string shaderCode) {
		m_shaderCodeStr = std::make_optional(shaderCode);
		return this;
	}

	VulkanShaderModuleBuilder* VulkanShaderModuleBuilder::SetShaderType(Graphics::ShaderType shaderType) {
		m_shaderType = std::make_optional(shaderType);
		return this;
	}

	std::shared_ptr<JarShaderModule> VulkanShaderModuleBuilder::Build(std::shared_ptr<JarDevice> device) {

		if (!m_shaderType.has_value() || !m_shaderCodeStr.has_value())
			throw std::runtime_error("Could not build shader module without code and type");

		auto vulkanDevice = reinterpret_cast<std::shared_ptr<VulkanDevice>&>(device);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_shaderCodeStr.value().size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(m_shaderCodeStr.value().data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vulkanDevice->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!");
		}

		return std::make_shared<VulkanShaderModule>(vulkanDevice, shaderModule, m_shaderType.value());
	}
}