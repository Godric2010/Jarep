//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALPIPELINEBUILDER_HPP
#define JAREP_METALPIPELINEBUILDER_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalDescriptorContent.hpp"
#include "MetalDataTypeMaps.hpp"
#include "MetalDevice.hpp"
#include "MetalPipeline.hpp"
#include "MetalShaderLibrary.hpp"

namespace Graphics::Metal {


	class MetalPipelineBuilder final : public JarPipelineBuilder {
		public:
			MetalPipelineBuilder() = default;

			~MetalPipelineBuilder() override;

			MetalPipelineBuilder* SetShaderStage(ShaderStage shaderStage) override;

			MetalPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			MetalPipelineBuilder* SetVertexInput(VertexInput vertexInput) override;

			MetalPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) override;

			MetalPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) override;

			MetalPipelineBuilder*
			BindDescriptorLayouts(std::vector<std::shared_ptr<JarDescriptorLayout>> descriptors) override;

			MetalPipelineBuilder* SetColorBlendAttachments(ColorBlendAttachment blendAttachment) override;

			MetalPipelineBuilder* SetDepthStencilState(DepthStencilState depthStencilState) override;

			std::shared_ptr<JarPipeline> Build(std::shared_ptr<JarDevice> device) override;

		private:
			MTL::Function* m_vertexShaderFunc;
			MTL::Function* m_fragmentShaderFunc;

			std::shared_ptr<JarRenderPass> m_renderPass;
			std::vector<MetalImageDescriptorContent> m_textureDescriptorBindings;

			MTL::VertexDescriptor* m_vertexDescriptor;
			MTL::PrimitiveTopologyClass m_topology;

			uint16_t m_multisamplingCount;

			std::vector<MTL::RenderPipelineColorAttachmentDescriptor*> m_colorAttachments;
			MTL::DepthStencilDescriptor* m_depthStencilDescriptor;
			MTL::StencilDescriptor* m_stencilDescriptor;

			MTL::ColorWriteMask convertToMetalColorWriteMask(ColorWriteMask mask);
	};

}


#endif //JAREP_METALPIPELINEBUILDER_HPP
