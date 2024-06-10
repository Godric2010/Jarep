//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANSHADERMODULEBUILDER_HPP
#define JAREP_VULKANSHADERMODULEBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanShaderModule.hpp"
#include <optional>
#include <memory>

namespace Graphics::Vulkan {
	class VulkanShaderModuleBuilder final : public JarShaderModuleBuilder {
		public:
			VulkanShaderModuleBuilder() = default;

			~VulkanShaderModuleBuilder() override;

			VulkanShaderModuleBuilder* SetShader(std::string shaderCode) override;

			VulkanShaderModuleBuilder* SetShaderType(ShaderType shaderType) override;

			std::shared_ptr <JarShaderModule> Build(std::shared_ptr <JarDevice> device) override;

		private:
			std::optional <std::string> m_shaderCodeStr;
			std::optional <ShaderType> m_shaderType;

	};
}

#endif //JAREP_VULKANSHADERMODULEBUILDER_HPP
