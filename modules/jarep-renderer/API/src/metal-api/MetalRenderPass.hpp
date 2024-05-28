//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALRENDERPASS_HPP
#define JAREP_METALRENDERPASS_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalRenderPassImages.hpp"
#include "MetalSurface.hpp"

namespace Graphics::Metal {

	class MetalRenderPass final : public JarRenderPass {
		public:
			explicit MetalRenderPass(MTL::RenderPassDescriptor* rpd, MetalRenderPassImages* renderPassImages) : renderPassDesc(rpd), renderPassImages(renderPassImages) {
			}

			~MetalRenderPass() override;

			void Release() override;

			void RecreateRenderPassFramebuffers(uint32_t width, uint32_t height, std::shared_ptr<JarSurface> metalSurface) override;

			[[nodiscard]] MTL::RenderPassDescriptor* getRenderPassDesc() const { return renderPassDesc; }

			void UpdateRenderPassDescriptor(std::shared_ptr<MetalSurface> metalSurface);

		private:
			MTL::RenderPassDescriptor* renderPassDesc;
			MetalRenderPassImages* renderPassImages;
	};
}


#endif //JAREP_METALRENDERPASS_HPP
