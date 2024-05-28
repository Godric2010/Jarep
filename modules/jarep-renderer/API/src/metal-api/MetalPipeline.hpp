//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALPIPELINE_HPP
#define JAREP_METALPIPELINE_HPP

#include <Metal/Metal.hpp>
#include <optional>
#include <memory>
#include "IRenderAPI.hpp"
#include "MetalDescriptorContent.hpp"

namespace Graphics::Metal {

	class MetalPipeline final : public JarPipeline {
		public:
			MetalPipeline(MTL::Device* device, MTL::RenderPipelineState* pipelineState,
			              std::optional<MTL::DepthStencilState*> depthStencilState,
			              std::shared_ptr<JarRenderPass> renderPass) :
					m_device(device), m_pipelineState(pipelineState),
					m_depthStencilState(depthStencilState), m_renderPass(std::move(renderPass)) {};

			~MetalPipeline() override;

			void Release() override;

			std::shared_ptr<JarRenderPass> GetRenderPass() override { return m_renderPass; }

			MTL::RenderPipelineState* getPSO() { return m_pipelineState; }

			std::optional<MTL::DepthStencilState*> getDSS() { return m_depthStencilState; }

		private:
			MTL::RenderPipelineState* m_pipelineState;
			std::optional<MTL::DepthStencilState*> m_depthStencilState;
			MTL::Device* m_device;
			std::shared_ptr<JarRenderPass> m_renderPass;
			std::vector<MetalImageDescriptorContent> m_textureDescriptorBindings;
	};

}
#endif //JAREP_METALPIPELINE_HPP
