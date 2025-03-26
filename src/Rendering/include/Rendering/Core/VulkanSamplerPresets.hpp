//
// Created by Godri on 3/26/2025.
//

#pragma once
#include <Rendering/Core/VulkanSamplerConfig.hpp>

namespace JAREP::Rendering::Core {
	SamplerConfig LinearRepeat();

	SamplerConfig NearestClamp();

	SamplerConfig ShadowSampler();

	SamplerConfig AnisotropicSampler();
}
