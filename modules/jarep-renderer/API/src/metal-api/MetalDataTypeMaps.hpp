//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALDATATYPEMAPS_HPP
#define JAREP_METALDATATYPEMAPS_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {


	static std::unordered_map<PixelFormat, MTL::PixelFormat> pixelFormatMap{
			{PixelFormat::R8Unorm,              MTL::PixelFormatR8Unorm},
			{PixelFormat::RG8Unorm,             MTL::PixelFormatRG8Unorm},
			{PixelFormat::RGBA8Unorm,           MTL::PixelFormatRGBA8Unorm},
			{PixelFormat::BGRA8Unorm,           MTL::PixelFormatBGRA8Unorm},
			{PixelFormat::R16Unorm,             MTL::PixelFormatR16Unorm},
			{PixelFormat::RG16Unorm,            MTL::PixelFormatRG16Unorm},
			{PixelFormat::RGBA16Unorm,          MTL::PixelFormatRGBA16Unorm},
			{PixelFormat::RGBA16Float,          MTL::PixelFormatRGBA16Float},
			{PixelFormat::R32Float,             MTL::PixelFormatR32Float},
			{PixelFormat::RG32Float,            MTL::PixelFormatRG32Float},
			{PixelFormat::RGBA32Float,          MTL::PixelFormatRGBA32Float},
			{PixelFormat::Depth32Float,         MTL::PixelFormatDepth32Float},
			{PixelFormat::Depth24Stencil8,      MTL::PixelFormatDepth24Unorm_Stencil8},
			{PixelFormat::Depth32FloatStencil8, MTL::PixelFormatDepth32Float_Stencil8},
			{PixelFormat::Depth16Unorm,         MTL::PixelFormatDepth16Unorm}
	};


	static std::unordered_map<VertexInputRate, MTL::VertexStepFunction> vertexInputRateMap{
			{VertexInputRate::PerInstance, MTL::VertexStepFunctionPerInstance},
			{VertexInputRate::PerVertex,   MTL::VertexStepFunctionPerVertex},
	};

	static std::unordered_map<Graphics::VertexFormat, MTL::VertexFormat> vertexFormatMap{
			{VertexFormat::Float,  MTL::VertexFormatFloat},
			{VertexFormat::Float2, MTL::VertexFormatFloat2},
			{VertexFormat::Float3, MTL::VertexFormatFloat3},
			{VertexFormat::Float4, MTL::VertexFormatFloat4},
			{VertexFormat::Int,    MTL::VertexFormatInt},
			{VertexFormat::Int2,   MTL::VertexFormatInt2},
			{VertexFormat::Int3,   MTL::VertexFormatInt3},
			{VertexFormat::Int4,   MTL::VertexFormatInt4},
	};

	static std::unordered_map<InputAssemblyTopology, MTL::PrimitiveTopologyClass> topologyMap{
			{InputAssemblyTopology::PointList,     MTL::PrimitiveTopologyClassPoint},
			{InputAssemblyTopology::LineList,      MTL::PrimitiveTopologyClassLine},
			{InputAssemblyTopology::LineStrip,     MTL::PrimitiveTopologyClassLine},
			{InputAssemblyTopology::TriangleList,  MTL::PrimitiveTopologyClassTriangle},
			{InputAssemblyTopology::TriangleStrip, MTL::PrimitiveTopologyClassTriangle},
	};


	static std::unordered_map<BlendFactor, MTL::BlendFactor> blendFactorMap{
			{BlendFactor::Zero,                  MTL::BlendFactorZero},
			{BlendFactor::One,                   MTL::BlendFactorOne},
			{BlendFactor::SrcColor,              MTL::BlendFactorSourceColor},
			{BlendFactor::OneMinusSrcColor,      MTL::BlendFactorOneMinusSourceColor},
			{BlendFactor::DstColor,              MTL::BlendFactorDestinationColor},
			{BlendFactor::OneMinusDstColor,      MTL::BlendFactorOneMinusDestinationColor},
			{BlendFactor::SrcAlpha,              MTL::BlendFactorSourceAlpha},
			{BlendFactor::OneMinusSrcAlpha,      MTL::BlendFactorOneMinusSourceAlpha},
			{BlendFactor::DstAlpha,              MTL::BlendFactorDestinationAlpha},
			{BlendFactor::OneMinusDstAlpha,      MTL::BlendFactorOneMinusDestinationAlpha},
			{BlendFactor::ConstantColor,         MTL::BlendFactorBlendColor},
			{BlendFactor::OneMinusConstantColor, MTL::BlendFactorOneMinusBlendColor},
			{BlendFactor::ConstantAlpha,         MTL::BlendFactorBlendAlpha},
			{BlendFactor::OneMinusConstantAlpha, MTL::BlendFactorOneMinusBlendAlpha}
	};

	static std::unordered_map<BlendOperation, MTL::BlendOperation> blendOperationMap{
			{BlendOperation::Add,             MTL::BlendOperationAdd},
			{BlendOperation::Subtract,        MTL::BlendOperationSubtract},
			{BlendOperation::ReverseSubtract, MTL::BlendOperationReverseSubtract},
			{BlendOperation::Min,             MTL::BlendOperationMin},
			{BlendOperation::Max,             MTL::BlendOperationMax},
	};

	static std::unordered_map<CompareOperation, MTL::CompareFunction> compareFuncMap{
			{CompareOperation::Never,        MTL::CompareFunctionNever},
			{CompareOperation::Less,         MTL::CompareFunctionLess},
			{CompareOperation::LessEqual,    MTL::CompareFunctionLessEqual},
			{CompareOperation::Equal,        MTL::CompareFunctionEqual},
			{CompareOperation::Greater,      MTL::CompareFunctionGreater},
			{CompareOperation::GreaterEqual, MTL::CompareFunctionGreaterEqual},
			{CompareOperation::NotEqual,     MTL::CompareFunctionNotEqual},
			{CompareOperation::AllTime,      MTL::CompareFunctionAlways}
	};

	static std::unordered_map<StencilOpState, MTL::StencilOperation> stencilOpMap{
			{StencilOpState::Keep,              MTL::StencilOperationKeep},
			{StencilOpState::Zero,              MTL::StencilOperationZero},
			{StencilOpState::Replace,           MTL::StencilOperationReplace},
			{StencilOpState::IncrementAndClamp, MTL::StencilOperationIncrementClamp},
			{StencilOpState::IncrementAndWrap,  MTL::StencilOperationIncrementWrap},
			{StencilOpState::DecrementAndClamp, MTL::StencilOperationDecrementClamp},
			{StencilOpState::DecrementAndWrap,  MTL::StencilOperationDecrementWrap},
			{StencilOpState::Invert,            MTL::StencilOperationInvert},
	};
}
#endif //JAREP_METALDATATYPEMAPS_HPP
