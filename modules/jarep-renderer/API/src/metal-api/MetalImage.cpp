//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalImage.hpp"

namespace Graphics::Metal {

	MetalImage::~MetalImage() =
	default;

	void MetalImage::Release() {
		m_texture->release();
	}
}