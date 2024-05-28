//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALRENDERPASSBUILDER_HPP
#define JAREP_METALRENDERPASSBUILDER_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalSurface.hpp"
#include "MetalRenderPass.hpp"
#include "MetalRenderPassImages.hpp"
#include "MetalDataTypeMaps.hpp"

namespace Graphics::Metal {

	class MetalRenderPassBuilder : public JarRenderPassBuilder {
		public:
			MetalRenderPassBuilder();

			~MetalRenderPassBuilder() override;

			JarRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) override;

			JarRenderPassBuilder* AddDepthStencilAttachment(DepthAttachment depthStencilAttachment) override;

			JarRenderPassBuilder* SetMultisamplingCount(uint8_t multisamplingCount) override;

			std::shared_ptr<JarRenderPass>
			Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) override;

		private:
			MTL::RenderPassDescriptor* m_renderPassDescriptor;
			std::optional<ColorAttachment> m_colorAttachment;
			std::optional<MTL::PixelFormat> m_depthStencilFormat;
			std::optional<uint8_t> m_multisamplingCount;

			static MTL::StoreAction storeActionToMetal(const StoreOp storeOp) {
				switch (storeOp) {
					case StoreOp::Store:
						return MTL::StoreActionStore;
					case StoreOp::DontCare:
						return MTL::StoreActionDontCare;
				}
				return {};
			}

			static MTL::LoadAction loadActionToMetal(const LoadOp loadOp) {
				switch (loadOp) {
					case LoadOp::Load:
						return MTL::LoadActionLoad;
					case LoadOp::Clear:
						return MTL::LoadActionClear;
					case LoadOp::DontCare:
						return MTL::LoadActionDontCare;
				}
				return {};
			}

			static MTL::ClearColor clearColorToMetal(ClearColor clearColor) {
				return {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
			}
	};
}


#endif //JAREP_METALRENDERPASSBUILDER_HPP
