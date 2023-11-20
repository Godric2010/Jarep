//
// Created by Sebastian Borsch on 19.11.23.
//

#include "sdlsurfaceadapter.hpp"
#import<AppKit/AppKit.h>
#include "AppKit/AppKit.hpp"
#import<QuartzCore/CAMetalLayer.h>

namespace Graphics::Metal {

	void
	SDLSurfaceAdapter::CreateViewAndMetalLayer(CGRect surfaceRect, NS::View **viewContent,
	                                           CA::MetalLayer **metalLayer) {
		NSView *nsView = [[NSView alloc] initWithFrame:NSMakeRect(surfaceRect.origin.x, surfaceRect.origin.y,
		                                                          surfaceRect.size.width, surfaceRect.size.height)];

		CAMetalLayer *mLayer = [CAMetalLayer layer];
		[mLayer setDrawableSize:surfaceRect.size];
		[nsView setLayer:mLayer];
		[nsView setWantsLayer:YES];
		nsView.layer.opaque = YES;

		*viewContent = ( __bridge NS::View *) nsView;
		*metalLayer = ( __bridge CA::MetalLayer *) mLayer;

	}

}