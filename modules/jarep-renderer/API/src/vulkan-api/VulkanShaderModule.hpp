//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANSHADERMODULE_HPP
#define JAREP_VULKANSHADERMODULE_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanDevice;

	class VulkanShaderModule final : public JarShaderModule {
		public:
			VulkanShaderModule(std::shared_ptr<VulkanDevice>& device, VkShaderModule shaderModule,
			                   ShaderType shaderType) : m_device(device), m_shaderModule(shaderModule),
			                                            m_shaderType(shaderType) {};

			~VulkanShaderModule() override;

			void Release() override;

			[[nodiscard]] VkShaderModule getShaderModule() const { return m_shaderModule; }

			[[nodiscard]] ShaderType getShaderType() const { return m_shaderType; }

		private:
			VkShaderModule m_shaderModule;
			ShaderType m_shaderType;
			std::shared_ptr<VulkanDevice> m_device;
	};
}

#endif //JAREP_VULKANSHADERMODULE_HPP
