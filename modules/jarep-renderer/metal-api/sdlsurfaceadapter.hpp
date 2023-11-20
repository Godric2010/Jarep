//
// Created by Sebastian Borsch on 19.11.23.
//

#ifndef SDLSURFACEADAPTER_HPP
#define SDLSURFACEADAPTER_HPP


#include <AppKit/AppKit.hpp>
#import "QuartzCore/CAMetalLayer.hpp"

namespace Graphics::Metal {

	class SDLSurfaceAdapter {
		public:
			SDLSurfaceAdapter() = default;

			~SDLSurfaceAdapter() = default;

			static void CreateViewAndMetalLayer(CGRect surfaceRect, NS::View** viewContent, CA::MetalLayer** metalLayer);
	};
}
#endif //SDLSURFACEADAPTER_HPP
