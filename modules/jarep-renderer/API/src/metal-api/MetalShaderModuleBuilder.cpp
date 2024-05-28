//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalShaderModuleBuilder.hpp"

namespace Graphics::Metal {

	MetalShaderLibraryBuilder::~MetalShaderLibraryBuilder() =
	default;

	MetalShaderLibraryBuilder* MetalShaderLibraryBuilder::SetShader(std::string shaderCode) {
		NS::String* shaderStr = NS::String::string(shaderCode.c_str(), NS::UTF8StringEncoding);
		m_shaderCodeString = std::make_optional(shaderStr);
		return this;
	}

	MetalShaderLibraryBuilder* MetalShaderLibraryBuilder::SetShaderType(ShaderType shaderType) {
		m_shaderTypeOpt = std::make_optional(shaderType);
		return this;
	}

	std::shared_ptr<JarShaderModule> MetalShaderLibraryBuilder::Build(std::shared_ptr<JarDevice> device) {
		const auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);

		if (!m_shaderCodeString.has_value() || !m_shaderTypeOpt.has_value())
			throw std::runtime_error("Could not build shader module! Shader type and/or code are undefined!");

		NS::Error* error = nullptr;
		const auto library = metalDevice->getDevice().value()->newLibrary(m_shaderCodeString.value(), nullptr,
		                                                                  &error);
		if (!library) {
			throw std::runtime_error("Failed to load vertex shader library: " +
			                         std::string(error->localizedDescription()->cString(NS::UTF8StringEncoding)));
		}

		return std::make_shared<MetalShaderLibrary>(library);
	}
}