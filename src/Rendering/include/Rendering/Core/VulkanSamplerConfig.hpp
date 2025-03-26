//
// Created by Godri on 3/26/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Core {
	struct SamplerConfig {
		VkFilter magFilter = VK_FILTER_LINEAR;
		VkFilter minFilter = VK_FILTER_LINEAR;

		VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		float mipLodBias = 0.0f;
		float minLod = 0.0f;
		float maxLod = VK_LOD_CLAMP_NONE;

		bool anisotropyEnable = false;
		float maxAnisotropy = 1.0f;

		bool unnormalizedCoordinates = false;

		bool compareEnable = false;
		VkCompareOp compareOp = VK_COMPARE_OP_LESS;

		VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	};
}
