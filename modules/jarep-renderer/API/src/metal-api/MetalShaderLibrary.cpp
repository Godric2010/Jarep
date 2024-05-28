//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalShaderLibrary.hpp"

namespace Graphics::Metal {

	MetalShaderLibrary::~MetalShaderLibrary() = default;

	void MetalShaderLibrary::Release() {
		m_library->release();
	}
}