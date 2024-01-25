//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

#include <complex>

#include "NativeWindowHandleProvider.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Graphics {
	class JarDevice;

	class JarShaderModule;

	class JarRenderPass;

	class JarCommandQueue;

	class JarCommandBuffer;

	class JarPipeline;

	class JarBuffer;

	class JarSurface;

	enum ImageFormat {
		B8G8R8A8_UNORM,
	};
#pragma region JarRenderPass{

	enum class StoreOp {
		Store,
		DontCare,
	};

	enum class LoadOp {
		Load,
		Clear,
		DontCare,
	};

	struct ClearColor {
		public:
			float r;
			float g;
			float b;
			float a;

			ClearColor(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {
			}
	};

	struct ColorAttachment {
		public:
			ImageFormat Format;
			LoadOp LoadOperation;
			StoreOp StoreOperation;
			ClearColor Clear;

			ColorAttachment() : Clear(0, 0, 0, 0), Format(ImageFormat::B8G8R8A8_UNORM), LoadOperation(LoadOp::Clear),
			                    StoreOperation(StoreOp::DontCare) {
			}
	};

	//	struct DepthStencilAttachment{
	//		public:
	//			ImageFormat ImageFormat;
	//			StoreOperation StencilStoreOp;
	//			LoadOperation StencilLoadOp;
	//			float DepthClearValue;
	//			uint32_t StencilClearValue;
	//	};

	class JarRenderPassBuilder {
		public:
			virtual ~JarRenderPassBuilder() = default;

			virtual JarRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) = 0;

			virtual std::shared_ptr<JarRenderPass>
			Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) = 0;
	};


	class JarRenderPass {
		public:
			virtual ~JarRenderPass() = default;

			virtual void Release() = 0;
	};

#pragma endregion JarRenderPass }

	class JarFramebuffer {
		public:
			virtual ~JarFramebuffer() = default;

			virtual void Release(std::shared_ptr<JarDevice> device) = 0;
	};

	class JarSurface {
		public:
			virtual ~JarSurface() = default;

			virtual void Update() = 0;
	};


#pragma region Buffer{

	enum class BufferUsage {
		VertexBuffer,
		IndexBuffer,
		UniformBuffer,
		StoreBuffer,
		TransferSrc,
		TransferDest,
	};

	enum class MemoryProperties {
		HostVisible,
		HostCoherent,
		HostCached,
		DeviceLocal,
		LazilyAllocation,
	};

	class JarBufferBuilder {
		public:
			virtual ~JarBufferBuilder() = default;

			virtual JarBufferBuilder* SetUsageFlags(BufferUsage usageFlags) = 0;

			virtual JarBufferBuilder* SetMemoryProperties(MemoryProperties memProps) = 0;

			virtual JarBufferBuilder* SetBufferData(const void* data, size_t bufferSize) = 0;

			virtual std::shared_ptr<JarBuffer> Build(std::shared_ptr<JarDevice> device) = 0;
	};

	class JarBuffer {
		public:
			virtual ~JarBuffer() = default;
	};

#pragma endregion Buffer }

#pragma region JarShader{
	enum ShaderType {
		VertexShader,
		FragmentShader,
		GeometryShader,
		ComputeShader,
	};

	class JarShaderModuleBuilder {
		public:
			virtual ~JarShaderModuleBuilder() = default;

			virtual JarShaderModuleBuilder* SetShader(std::string shaderCode) = 0;

			virtual JarShaderModuleBuilder* SetShaderType(ShaderType shaderType) = 0;

			virtual std::shared_ptr<JarShaderModule> Build(std::shared_ptr<JarDevice> device) = 0;
	};

	class JarShaderModule {
		public:
			virtual ~JarShaderModule() = default;

			virtual void Release() = 0;
	};

#pragma endregion JarShader }

#pragma region JarPipeline{

	struct ShaderStage {
		std::shared_ptr<JarShaderModule> VertexShaderModule;
		std::shared_ptr<JarShaderModule> FragmentShaderModule;
		std::string MainFunctionName;

	};

	enum class VertexInputRate {
		PerVertex,
		PerInstance,
	};

	struct BindingDescription {
		uint32_t BindingIndex;
		uint32_t Stride;
		VertexInputRate InputRate;
		uint32_t StepRate;
	};

	enum class VertexFormat {
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
	};

	struct AttributeDescription {
		uint32_t BindingIndex;
		uint32_t AttributeLocation;
		uint32_t Offset;
		VertexFormat Format;
	};

	struct VertexInput {
		std::vector<BindingDescription> BindingDescriptions;
		std::vector<AttributeDescription> AttributeDescriptions;
	};

	enum class InputAssemblyTopology {
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
	};

	enum class PolygonMode {
	};

	enum class CullMode {
	};

	enum class FrontFace {
	};

	struct RasterizationState {
		PolygonMode polygonMode;
		CullMode cullMode;
		FrontFace frontFace;
	};

	enum class PixelFormat {
		RGBA8_UNORM,
		BGRA8_UNORM,
		RGBA16_FLOAT,
		RGBA32_FLOAT,
		DEPTH32_FLOAT,
		DEPTH24_STENCIL8,
		R8_UNORM,
		R16_FLOAT,
		BC1,
		BC3,
		PVRTC,
	};

	enum class BlendFactor {
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
	};

	enum class BlendOperation {
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	enum class ColorWriteMask {
		Non = 0,
		Red = 1 << 0,
		Green = 1 << 1,
		Blue = 1 << 2,
		Alpha = 1 << 3,
		All = Red | Green | Blue | Alpha
	};

	struct ColorBlendAttachment {
		PixelFormat Format;
		bool BlendingEnabled;
		BlendFactor SourceRGBBlendFactor;
		BlendFactor DestinationRGBBlendFactor;
		BlendOperation RGBBlendOperation;
		BlendFactor SourceAlphaBlendFactor;
		BlendFactor DestinationAlphaBlendFactor;
		BlendOperation AlphaBlendOperation;
		ColorWriteMask WriteMask;
	};

	enum class DepthCompareOperation {
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		AllTime,
	};

	enum class StencilOpState {
		Keep,
		Zero,
		Replace,
		IncrementAndClamp,
		DecrementAndClamp,
		Invert,
		IncrementAndWrap,
		DecrementAndWrap,
	};

	struct DepthStencilState {
		bool DepthTestEnable;
		bool DepthWriteEnable;
		DepthCompareOperation DepthCompareOp;
		bool StencilTestEnable;
		StencilOpState StencilOpState;
	};

	class JarPipelineBuilder {
		public:
			virtual ~JarPipelineBuilder() = default;

			virtual JarPipelineBuilder* SetShaderStage(ShaderStage shaderStage) = 0;

			virtual JarPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) = 0;

			virtual JarPipelineBuilder* SetVertexInput(VertexInput vertexInput) = 0;

			virtual JarPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) = 0;

			virtual JarPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) = 0;

			virtual JarPipelineBuilder* SetColorBlendAttachments(ColorBlendAttachment colorBlendAttachments) = 0;

			virtual JarPipelineBuilder* SetDepthStencilState(DepthStencilState depthStencilState) = 0;

			virtual std::shared_ptr<JarPipeline> Build(std::shared_ptr<JarDevice> device) = 0;
	};

	class JarPipeline {
		public:
			virtual ~JarPipeline() = default;

			virtual void Release() = 0;

			virtual std::shared_ptr<JarRenderPass> GetRenderPass() = 0;
	};

#pragma endregion JarPipeline }

#pragma region JarCommandBuffer{

	class JarCommandBuffer {
		public:
			virtual ~JarCommandBuffer() = default;

			virtual void
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) = 0;

			virtual void EndRecording() = 0;

			virtual void BindPipeline(std::shared_ptr<JarPipeline> pipeline) = 0;

			virtual void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) = 0;

			virtual void Draw() = 0;

			virtual void Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) = 0;
	};

#pragma endregion JarCommandBuffer }

#pragma region CommandQueue{

	class JarCommandQueueBuilder {
		public:
			virtual ~JarCommandQueueBuilder() = default;

			virtual JarCommandQueueBuilder* SetCommandBufferAmount(uint32_t commandBufferAmount) = 0;

			virtual std::shared_ptr<JarCommandQueue> Build(std::shared_ptr<JarDevice> device) = 0;
	};

	class JarCommandQueue {
		public:
			virtual ~JarCommandQueue() = default;

			virtual JarCommandBuffer* getNextCommandBuffer() = 0;

			virtual void Release() = 0;
	};

#pragma endregion CommandQueue }

	class JarDevice {
		public:
			virtual ~JarDevice() = default;

			virtual void Release() = 0;
	};

#pragma region Backend{

	class Backend {
		public:
			virtual ~Backend() = default;

			virtual std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* windowHandleProvider) = 0;

			virtual std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& surface) = 0;

			virtual JarShaderModuleBuilder* InitShaderModuleBuilder() = 0;

			virtual JarRenderPassBuilder* InitRenderPassBuilder() = 0;

			virtual JarCommandQueueBuilder* InitCommandQueueBuilder() = 0;

			virtual JarBufferBuilder* InitBufferBuilder() = 0;

			virtual JarPipelineBuilder* InitPipelineBuilder() = 0;
	};

#pragma endregion Backend }
}
#endif //JAREP_IRENDERER_HPP
