//
// Created by Godri on 3/26/2025.
//

#include <Rendering/Core/VulkanSamplerPresets.hpp>

using namespace JAREP::Rendering::Core;

SamplerConfig LinearRepeat() {
	SamplerConfig config{};
	config.minFilter = VK_FILTER_LINEAR;
	config.magFilter = VK_FILTER_LINEAR;
	config.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	config.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	config.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	return config;
}

SamplerConfig NearestClamp() {
	SamplerConfig config{};
	config.minFilter = VK_FILTER_NEAREST;
	config.magFilter = VK_FILTER_NEAREST;
	config.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	config.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	config.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	config.anisotropyEnable = false;
	config.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	return config;
}

SamplerConfig ShadowSampler() {
	SamplerConfig config{};
	config.compareEnable = true;
	config.compareOp = VK_COMPARE_OP_LESS;
	config.unnormalizedCoordinates = true;
	config.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	config.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	config.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	config.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	config.anisotropyEnable = false;
	config.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	return config;
}

SamplerConfig AnisotropicSampler() {
	SamplerConfig config{};
	config.anisotropyEnable = true;
	config.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	config.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	config.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	return config;
}
