//
// Created by Sebastian Borsch on 30.03.24.
//

#ifndef JAREP_JARRENDERSTEP_HPP
#define JAREP_JARRENDERSTEP_HPP

#include <iostream>
#include <memory>
#include <utility>
#include <filesystem>
#include <fstream>
#include "API/src/IRendererAPI/IRenderAPI.hpp"
#include "Vertex.hpp"


namespace Graphics {

	class JarRenderStepDescriptor {
		public:
			JarRenderStepDescriptor() = default;

			~JarRenderStepDescriptor() = default;

			std::string m_vertexShaderName;
			std::string m_fragmentShaderName;
			bool m_depthTestEnabled;
			bool m_stencilTestEnabled;
	};

	namespace Internal {
		class JarRenderStep {
			public:
				JarRenderStep(std::unique_ptr<JarRenderStepDescriptor> desc, std::shared_ptr<Backend> backend,
				              std::shared_ptr<JarDevice> device, std::shared_ptr<JarRenderTarget> renderTarget,
				              std::shared_ptr<JarSurface> surface,
				              std::vector<std::shared_ptr<JarDescriptor>> descriptors,
				              std::shared_ptr<JarImageBuffer> multisamplingImageAttachment,
				              std::shared_ptr<JarImageBuffer> depthImageAttachment);

				~JarRenderStep() = default;

				void Release();

			    void ResizeFramebuffer(const std::shared_ptr<Backend>& backend, std::shared_ptr<JarDevice> device,
			                           const std::shared_ptr<JarRenderTarget>& renderTarget,
			                           std::shared_ptr<JarImageBuffer> multisamplingImageAttachment,
			                           std::shared_ptr<JarImageBuffer> depthImageAttachment);

				[[nodiscard]] std::shared_ptr<JarPipeline> GetPipeline() const { return m_pipeline; }

				[[nodiscard]] std::shared_ptr<JarRenderPass> GetRenderPass() const { return m_renderPass; }

				[[nodiscard]] std::vector<std::shared_ptr<JarDescriptor>>
				GetDescriptors() const { return m_descriptors; }

				[[nodiscard]] std::shared_ptr<JarFramebuffer> GetFramebuffer() const { return m_framebuffer; }

			private:
				ShaderStage m_shaderStage;
				std::shared_ptr<JarPipeline> m_pipeline;
				std::shared_ptr<JarRenderPass> m_renderPass;
				std::shared_ptr<JarFramebuffer> m_framebuffer;
				std::vector<std::shared_ptr<JarDescriptor>> m_descriptors;

				std::unique_ptr<JarRenderStepDescriptor> renderStepDescriptor;
				std::shared_ptr<JarRenderTarget> m_renderTarget;

				void BuildShaderModules(std::shared_ptr<Backend> backend, std::shared_ptr<JarDevice> device);

			    void BuildFramebuffer(const std::shared_ptr<Backend>& backend, std::shared_ptr<JarDevice> device, const std::shared_ptr<JarRenderTarget>& renderTarget, const std::shared_ptr<JarImageBuffer>& multisamplingImageAttachment, const std::shared_ptr<JarImageBuffer>& depthImageAttachment);

			    void BuildRenderPass(const std::shared_ptr<Backend>& backend, std::shared_ptr<JarSurface> surface, std::shared_ptr<JarDevice> device);

				void BuildPipeline(const std::shared_ptr<Backend>& backend, std::shared_ptr<JarDevice> device,
				                   std::vector<std::shared_ptr<JarDescriptor>> descriptors);

				std::shared_ptr<JarShaderModule>
				GetShaderModule(const std::string& shaderName, ShaderType type, std::shared_ptr<Backend> backend,
				                std::shared_ptr<JarDevice> device);

				static std::string ReadFile(const std::string& filename);


		};
	}

} // Graphics

#endif //JAREP_JARRENDERSTEP_HPP
