//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalPipeline.hpp"

namespace Graphics::Metal{

	MetalPipeline::~MetalPipeline() =
	default;

	void MetalPipeline::Release() {
		m_pipelineState->release();
	}
}