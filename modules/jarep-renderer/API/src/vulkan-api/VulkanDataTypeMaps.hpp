//
// Created by sebastian on 09.06.24.
//
#pragma once
#ifndef JAREP_VULKANDATATYPEMAPS_HPP
#define JAREP_VULKANDATATYPEMAPS_HPP

#include "IRenderAPI.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace Graphics::Vulkan {

	static std::unordered_map<PixelFormat, VkFormat> pixelFormatMap = {
			{PixelFormat::R8Unorm,              VK_FORMAT_R8_UNORM},
			{PixelFormat::RG8Unorm,             VK_FORMAT_R8G8_UNORM},
			{PixelFormat::RGBA8Unorm,           VK_FORMAT_R8G8B8A8_UNORM},
			{PixelFormat::BGRA8Unorm,           VK_FORMAT_B8G8R8A8_UNORM},
			{PixelFormat::R16Unorm,             VK_FORMAT_R16_UNORM},
			{PixelFormat::RG16Unorm,            VK_FORMAT_R16G16_UNORM},
			{PixelFormat::RGBA16Unorm,          VK_FORMAT_R16G16B16A16_UNORM},
			{PixelFormat::RGBA16Float,          VK_FORMAT_R16G16B16A16_SFLOAT},
			{PixelFormat::R32Float,             VK_FORMAT_R32_SFLOAT},
			{PixelFormat::RG32Float,            VK_FORMAT_R32G32_SFLOAT},
			{PixelFormat::RGBA32Float,          VK_FORMAT_R32G32B32A32_SFLOAT},
			{PixelFormat::Depth32Float,         VK_FORMAT_D32_SFLOAT},
			{PixelFormat::Depth24Stencil8,      VK_FORMAT_D24_UNORM_S8_UINT},
			{PixelFormat::Depth16Unorm,         VK_FORMAT_D16_UNORM},
			{PixelFormat::Depth32FloatStencil8, VK_FORMAT_D32_SFLOAT_S8_UINT},
	};
	static std::unordered_map<StoreOp, VkAttachmentStoreOp> storeOpMap = {
			{StoreOp::Store,    VK_ATTACHMENT_STORE_OP_STORE},
			{StoreOp::DontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE},
	};

	static std::unordered_map<LoadOp, VkAttachmentLoadOp> loadOpMap = {
			{LoadOp::Load,     VK_ATTACHMENT_LOAD_OP_LOAD},
			{LoadOp::DontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE},
			{LoadOp::Clear,    VK_ATTACHMENT_LOAD_OP_CLEAR},
	};
	static std::unordered_map<VertexInputRate, VkVertexInputRate> inputRateMap = {
			{VertexInputRate::PerVertex,   VK_VERTEX_INPUT_RATE_VERTEX},
			{VertexInputRate::PerInstance, VK_VERTEX_INPUT_RATE_INSTANCE},
	};

	static std::unordered_map<VertexFormat, VkFormat> vertexFormatMap = {
			{VertexFormat::Float,  VK_FORMAT_R32_SFLOAT},
			{VertexFormat::Float2, VK_FORMAT_R32G32_SFLOAT},
			{VertexFormat::Float3, VK_FORMAT_R32G32B32_SFLOAT},
			{VertexFormat::Float4, VK_FORMAT_R32G32B32A32_SFLOAT},
			{VertexFormat::Int,    VK_FORMAT_R32_SINT},
			{VertexFormat::Int2,   VK_FORMAT_R32G32_SINT},
			{VertexFormat::Int3,   VK_FORMAT_R32G32B32_SINT},
			{VertexFormat::Int4,   VK_FORMAT_R32G32B32A32_SINT},
	};

	static std::unordered_map<InputAssemblyTopology, VkPrimitiveTopology> topologyMap = {
			{InputAssemblyTopology::LineList,      VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
			{InputAssemblyTopology::LineStrip,     VK_PRIMITIVE_TOPOLOGY_LINE_STRIP},
			{InputAssemblyTopology::PointList,     VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
			{InputAssemblyTopology::TriangleList,  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
			{InputAssemblyTopology::TriangleStrip, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP},
	};

	static std::unordered_map<BlendFactor, VkBlendFactor> blendFactorMap = {
			{BlendFactor::Zero,                  VK_BLEND_FACTOR_ZERO},
			{BlendFactor::One,                   VK_BLEND_FACTOR_ONE},
			{BlendFactor::SrcColor,              VK_BLEND_FACTOR_SRC_COLOR},
			{BlendFactor::OneMinusSrcColor,      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR},
			{BlendFactor::DstColor,              VK_BLEND_FACTOR_DST_COLOR},
			{BlendFactor::OneMinusDstColor,      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR},
			{BlendFactor::SrcAlpha,              VK_BLEND_FACTOR_SRC_ALPHA},
			{BlendFactor::OneMinusSrcAlpha,      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
			{BlendFactor::DstAlpha,              VK_BLEND_FACTOR_DST_ALPHA},
			{BlendFactor::OneMinusDstAlpha,      VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA},
			{BlendFactor::ConstantColor,         VK_BLEND_FACTOR_CONSTANT_COLOR},
			{BlendFactor::OneMinusConstantColor, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR},
			{BlendFactor::ConstantAlpha,         VK_BLEND_FACTOR_CONSTANT_ALPHA},
			{BlendFactor::OneMinusConstantAlpha, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA},
	};

	static std::unordered_map<BlendOperation, VkBlendOp> blendOpMap = {
			{BlendOperation::Add,             VK_BLEND_OP_ADD},
			{BlendOperation::Subtract,        VK_BLEND_OP_SUBTRACT},
			{BlendOperation::ReverseSubtract, VK_BLEND_OP_REVERSE_SUBTRACT},
			{BlendOperation::Min,             VK_BLEND_OP_MIN},
			{BlendOperation::Max,             VK_BLEND_OP_MAX},
	};

	static std::unordered_map<CompareOperation, VkCompareOp> compareOpMap = {
			{CompareOperation::Never,        VK_COMPARE_OP_NEVER},
			{CompareOperation::Less,         VK_COMPARE_OP_LESS},
			{CompareOperation::LessEqual,    VK_COMPARE_OP_LESS_OR_EQUAL},
			{CompareOperation::Equal,        VK_COMPARE_OP_EQUAL},
			{CompareOperation::GreaterEqual, VK_COMPARE_OP_GREATER_OR_EQUAL},
			{CompareOperation::Greater,      VK_COMPARE_OP_GREATER},
			{CompareOperation::NotEqual,     VK_COMPARE_OP_NOT_EQUAL},
			{CompareOperation::AllTime,      VK_COMPARE_OP_ALWAYS},
	};

	static std::unordered_map<StencilOpState, VkStencilOp> stencilCompareOpMap = {
			{StencilOpState::Zero,              VK_STENCIL_OP_ZERO},
			{StencilOpState::DecrementAndClamp, VK_STENCIL_OP_DECREMENT_AND_CLAMP},
			{StencilOpState::DecrementAndWrap,  VK_STENCIL_OP_DECREMENT_AND_WRAP},
			{StencilOpState::IncrementAndClamp, VK_STENCIL_OP_INCREMENT_AND_CLAMP},
			{StencilOpState::IncrementAndWrap,  VK_STENCIL_OP_INCREMENT_AND_WRAP},
			{StencilOpState::Invert,            VK_STENCIL_OP_INVERT},
			{StencilOpState::Keep,              VK_STENCIL_OP_KEEP},
			{StencilOpState::Replace,           VK_STENCIL_OP_REPLACE},
	};


	inline 	VkSampleCountFlagBits convertToVkSampleCountFlagBits(uint8_t sampleCount) {
		switch (sampleCount) {
			case 2:
				return VK_SAMPLE_COUNT_2_BIT;
			case 4:
				return VK_SAMPLE_COUNT_4_BIT;
			case 8:
				return VK_SAMPLE_COUNT_8_BIT;
			case 16:
				return VK_SAMPLE_COUNT_16_BIT;
			case 32:
				return VK_SAMPLE_COUNT_32_BIT;
			case 64:
				return VK_SAMPLE_COUNT_64_BIT;
			default:
				return VK_SAMPLE_COUNT_1_BIT;
		}
	}
}

#endif //JAREP_VULKANDATATYPEMAPS_HPP
