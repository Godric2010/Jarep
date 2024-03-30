//
// Created by Sebastian Borsch on 30.03.24.
//

#ifndef JAREP_JARRENDERSTEP_HPP
#define JAREP_JARRENDERSTEP_HPP

#include <iostream>
#include <memory>
#include <utility>
#include "IRenderAPI.hpp"


namespace Graphics {

	class JarRenderStepDescriptor {
		public:
			JarRenderStepDescriptor() = default;

			~JarRenderStepDescriptor() = default;

			std::string m_vertexShaderName;
			std::string m_fragmentShaderName;
			bool m_depthTestEnabled;
			bool m_stencilTestEnabled;
			uint32_t m_multisamplingCount;


	};

	namespace Internal {
		class JarRenderStep {
			public:
				JarRenderStep(std::unique_ptr<JarRenderStepDescriptor> desc, std::shared_ptr<JarDevice> device,
				              std::shared_ptr<JarSurface> surface) : descriptor(std::move(desc)) {};

				~JarRenderStep() = default;

			private:
				std::shared_ptr<JarShaderModule> vertexShaderModule;
				std::shared_ptr<JarShaderModule> fragmentShaderModule;
				std::shared_ptr<JarPipeline> pipeline;
				std::shared_ptr<JarRenderPass> renderPass;

				std::unique_ptr<JarRenderStepDescriptor> descriptor;

				void BuildShaderModules(std::shared_ptr<JarDevice> device);

				void BuildRenderPass(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarDevice> device);

				void BuildPipeline(std::shared_ptr<JarDevice> device);


		};
	}

} // Graphics

#endif //JAREP_JARRENDERSTEP_HPP
