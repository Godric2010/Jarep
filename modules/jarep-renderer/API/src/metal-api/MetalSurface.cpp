//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalSurface.hpp"

namespace Graphics::Metal {
	MetalSurface::MetalSurface() = default;

	MetalSurface::~MetalSurface() = default;


	bool MetalSurface::CreateFromNativeWindowProvider(NativeWindowHandleProvider* windowHandleProvider) {
		window = static_cast<NS::Window*>(windowHandleProvider->getNativeWindowHandle());


		surfaceRect = CGRectMake(0, 0, windowHandleProvider->getWindowWidth(),
		                         windowHandleProvider->getWindowHeight());
		return true;
	}

	void MetalSurface::RecreateSurface(uint32_t width, uint32_t height) {
		surfaceRect = CGRectMake(0, 0, width, height);
		layer->setDrawableSize(surfaceRect.size);
	}

	void MetalSurface::ReleaseSwapchain() {
	}

	uint32_t MetalSurface::GetSwapchainImageAmount() {
		return maxSwapchainImageCount;
	}

	JarExtent MetalSurface::GetSurfaceExtent() {
		return JarExtent{static_cast<float>(surfaceRect.size.width), static_cast<float>(surfaceRect.size.height)};
	}

	void MetalSurface::FinalizeSurface(std::shared_ptr<MetalDevice> device) {
		Graphics::Metal::SDLSurfaceAdapter::CreateViewAndMetalLayer(surfaceRect, &contentView, &layer,
		                                                            &maxSwapchainImageCount);

		metalDevice = device;
		if (contentView == nullptr)
			throw std::runtime_error("Expected NS::View* to be not nullptr!");

		if (layer == nullptr)
			throw std::runtime_error("Expected metal layer not to be nullptr");

		layer->setDevice(metalDevice->getDevice().value());
		layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		layer->setDrawableSize(surfaceRect.size);

		window->setContentView(contentView);
		SDLSurfaceAdapter::getDrawableFromMetalLayer(layer, &drawable);
	}

	MTL::PixelFormat MetalSurface::getDrawablePixelFormat() const {
		return drawable->texture()->pixelFormat();
	}
}