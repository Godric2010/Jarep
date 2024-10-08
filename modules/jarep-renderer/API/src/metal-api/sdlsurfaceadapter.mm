//
// Created by Sebastian Borsch on 19.11.23.
//

#if defined(__APPLE__)
#include "sdlsurfaceadapter.hpp"
#import<AppKit/AppKit.h>
#include "AppKit/AppKit.hpp"
#import<QuartzCore/CAMetalLayer.h>

namespace Graphics::Metal {

	void
	SDLSurfaceAdapter::CreateViewAndMetalLayer(CGRect surfaceRect, NS::View **viewContent,
	                                           CA::MetalLayer **metalLayer, uint32_t *maxSwapchainImageCount) {
		NSView *nsView = [[NSView alloc] initWithFrame:NSMakeRect(surfaceRect.origin.x, surfaceRect.origin.y,
		                                                          surfaceRect.size.width, surfaceRect.size.height)];

		CAMetalLayer *mLayer = [CAMetalLayer layer];
		[mLayer setDrawableSize:surfaceRect.size];
		[nsView setLayer:mLayer];
		[nsView setWantsLayer:YES];
		nsView.layer.opaque = YES;

		*viewContent = ( __bridge NS::View *) nsView;
		*metalLayer = ( __bridge CA::MetalLayer *) mLayer;
		*maxSwapchainImageCount = mLayer.maximumDrawableCount;
	}

	void SDLSurfaceAdapter::getDrawableFromMetalLayer(CA::MetalLayer* metalLayer, CA::MetalDrawable** drawable) {
		*drawable = ( __bridge CA::MetalDrawable *) [metalLayer nextDrawable];
	}


}
#endif