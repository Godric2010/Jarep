//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALSURFACE_HPP
#define JAREP_METALSURFACE_HPP

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include "IRenderAPI.hpp"
#include "MetalDevice.hpp"
#include "sdlsurfaceadapter.hpp"

namespace Graphics::Metal {

	class MetalSurface final : public JarSurface {
		public:
			MetalSurface();

			~MetalSurface() override;

			bool CreateFromNativeWindowProvider(NativeWindowHandleProvider* windowHandleProvider);

			void RecreateSurface(uint32_t width, uint32_t height) override;

			void ReleaseSwapchain() override;

			uint32_t GetSwapchainImageAmount() override;

			JarExtent GetSurfaceExtent() override;

			void FinalizeSurface(std::shared_ptr<MetalDevice> device);

			MTL::PixelFormat getDrawablePixelFormat() const;

			[[nodiscard]] CA::MetalDrawable* getDrawable() const { return drawable; }

			[[nodiscard]] bool isSurfaceInitialized() const { return contentView != nullptr; }

			MTL::Texture* acquireNewDrawTexture() {
				drawable = layer->nextDrawable();
				return drawable->texture();
			}

		private:
			std::shared_ptr<MetalDevice> metalDevice;
			NS::View* contentView;
			NS::Window* window;
			CGRect surfaceRect;
			CA::MetalLayer* layer;
			CA::MetalDrawable* drawable;
			uint32_t maxSwapchainImageCount;
	};
}


#endif //JAREP_METALSURFACE_HPP
