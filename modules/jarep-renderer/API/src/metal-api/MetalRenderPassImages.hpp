//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALRENDERPASSIMAGES_HPP
#define JAREP_METALRENDERPASSIMAGES_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalDevice.hpp"

namespace Graphics::Metal {

	class MetalRenderPassImages{
		public:
			MetalRenderPassImages(std::shared_ptr<MetalDevice> device): m_device(std::move(device)), m_msaaTexture(nullptr), m_depthStencilTexture(nullptr), m_multisamplingCount(1) {}
			~MetalRenderPassImages() = default;

			void CreateRenderPassImages(uint32_t width, uint32_t height, uint8_t multisamplingCount, MTL::PixelFormat msaaFormat, std::optional<MTL::PixelFormat> depthStencilFormat);
			void RecreateRenderPassImages(uint32_t width, uint32_t height, MTL::PixelFormat msaaFormat);

			void Release();

			[[nodiscard]] MTL::Texture* getMSAATexture() const { return m_msaaTexture; }
			[[nodiscard]] MTL::Texture* getDepthStencilTexture() const { return m_depthStencilTexture; }

		private:
			MTL::Texture* m_msaaTexture;
			MTL::Texture* m_depthStencilTexture;
			MTL::PixelFormat m_depthStencilFormat;
			uint8_t m_multisamplingCount;
			std::shared_ptr<MetalDevice> m_device;

			void createMSAATexture(uint32_t width, uint32_t height,MTL::PixelFormat format, uint8_t multisamplingCount);
			void createDepthStencilTexture(uint32_t width, uint32_t height, MTL::PixelFormat format, uint8_t multisamplingCount);
	};
}
#endif //JAREP_METALRENDERPASSIMAGES_HPP
