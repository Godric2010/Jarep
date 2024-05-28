//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALSHADERMODULEBUILDER_HPP
#define JAREP_METALSHADERMODULEBUILDER_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalDevice.hpp"
#include "MetalShaderLibrary.hpp"

namespace Graphics::Metal {

	class MetalShaderLibraryBuilder final : public JarShaderModuleBuilder {
		public:
			MetalShaderLibraryBuilder() = default;

			~MetalShaderLibraryBuilder() override;

			MetalShaderLibraryBuilder* SetShader(std::string shaderCode) override;

			MetalShaderLibraryBuilder* SetShaderType(ShaderType shaderType) override;

			std::shared_ptr<JarShaderModule> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<NS::String*> m_shaderCodeString;
			std::optional<ShaderType> m_shaderTypeOpt;
	};
}


#endif //JAREP_METALSHADERMODULEBUILDER_HPP
