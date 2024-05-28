//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALCOMMANDBUFFER_HPP
#define JAREP_METALCOMMANDBUFFER_HPP

#include "IRenderAPI.hpp"
#include "MetalRenderPass.hpp"
#include "MetalBuffer.hpp"
#include "MetalPipeline.hpp"
#include "MetalDescriptor.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {

	class MetalCommandBuffer final : public JarCommandBuffer {
		public:
			explicit MetalCommandBuffer(MTL::CommandBuffer* cmdBuffer) : buffer(cmdBuffer) {
				encoder = nullptr;
			}

			~MetalCommandBuffer() override;

			bool
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) override;

			void EndRecording() override;

			void BindPipeline(std::shared_ptr<JarPipeline> pipeline, uint32_t frameIndex) override;

			void BindDescriptors(std::vector<std::shared_ptr<JarDescriptor>> descriptors) override;

			void BindVertexBuffer(std::shared_ptr<JarBuffer> jarBuffer) override;

			void BindIndexBuffer(std::shared_ptr<JarBuffer> jarBuffer) override;

			void SetDepthBias(DepthBias depthBias) override;

			void SetViewport(Viewport viewport) override;

			void SetScissor(Scissor scissor) override;

			void Draw() override;

			void DrawIndexed(size_t indexAmount) override;

			void Present(std::shared_ptr<JarSurface>& m_surface, std::shared_ptr<JarDevice> device) override;

			MTL::CommandBuffer* getCommandBuffer() const { return buffer; }

		private:
			MTL::CommandBuffer* buffer;
			MTL::RenderCommandEncoder* encoder;
			std::shared_ptr<MetalBuffer> indexBuffer;
			std::shared_ptr<MetalRenderPass> metalRenderPass;
	};
}

#endif //JAREP_METALCOMMANDBUFFER_HPP
