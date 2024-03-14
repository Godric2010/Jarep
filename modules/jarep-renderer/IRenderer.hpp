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
#include <optional>

#include "stb_image.h"

namespace Graphics {
	class JarDevice;

	class JarShaderModule;

	class JarRenderPass;

	class JarCommandQueue;

	class JarCommandBuffer;

	class JarPipeline;

	class JarBuffer;

	class JarImage;

	class JarSurface;

	enum class PixelFormat;

	enum ImageFormat {
		B8G8R8A8_UNORM,
		D32_SFLOAT,
		D24_UNORM_S8_UINT,
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
			ImageFormat imageFormat;
			LoadOp loadOp;
			StoreOp storeOp;
			ClearColor clearColor;

			ColorAttachment() : clearColor(0, 0, 0, 0), imageFormat(ImageFormat::B8G8R8A8_UNORM), loadOp(LoadOp::Clear),
			                    storeOp(StoreOp::DontCare) {
			}
	};

	struct StencilAttachment {
		StoreOp StencilStoreOp;
		LoadOp StencilLoadOp;
		uint32_t StencilClearValue;
	};
	struct DepthAttachment {
		public:
			ImageFormat Format;
			StoreOp DepthStoreOp;
			LoadOp DepthLoadOp;
			float DepthClearValue;
			std::optional<StencilAttachment> Stencil;
	};


	class JarRenderPassBuilder {
		public:
			virtual ~JarRenderPassBuilder() = default;

			virtual JarRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) = 0;

			virtual JarRenderPassBuilder* AddDepthStencilAttachment(DepthAttachment depthStencilAttachment) = 0;

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

	struct JarExtent {
		public:
			float Width;
			float Height;
	};

	class JarSurface {
		public:
			virtual ~JarSurface() = default;

			virtual void Update() = 0;

			virtual void ReleaseSwapchain() = 0;

			virtual uint32_t GetSwapchainImageAmount() = 0;

			virtual JarExtent GetSurfaceExtent() = 0;
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

	class MemoryProperties {
		public:
			enum FlagBits {
				DeviceLocal = 0x1,
				HostVisible = 0x2,
				HostCoherent = 0x4,
				HostCached = 0x8,
				LazilyAllocation = 0x10,
			};

			using BitType = uint32_t;
			BitType flags;

			MemoryProperties() : flags(0) {}

			MemoryProperties(BitType flag) : flags(flag) {}

			MemoryProperties(std::initializer_list<BitType> flags) : flags(0) {
				for (auto flag: flags) {
					this->flags |= flag;
				}
			};

			MemoryProperties operator|(MemoryProperties rhs) const {
				return {static_cast<BitType>(this->flags | rhs.flags)};
			}

			MemoryProperties& operator|=(MemoryProperties rhs) {
				this->flags |= rhs.flags;
				return *this;
			}

			operator BitType() const { return flags; }
	};

	inline MemoryProperties operator|(MemoryProperties::FlagBits lhs, MemoryProperties::FlagBits rhs) {
		return MemoryProperties(
				static_cast<MemoryProperties::BitType>(lhs) | static_cast<MemoryProperties::BitType>(rhs));
	}

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

			virtual void Release() = 0;

			virtual void Update(const void* data, size_t bufferSize) = 0;
	};

#pragma endregion Buffer }

#pragma region JarImage{

	class JarImageBuilder {
		public:
			virtual ~JarImageBuilder() = default;

			virtual JarImageBuilder* SetPixelFormat(PixelFormat pixelFormat) = 0;

			virtual JarImageBuilder* SetImagePath(std::string imagePath) = 0;

			virtual std::shared_ptr<JarImage> Build(std::shared_ptr<JarDevice> device) = 0;
	};


	class JarImage {
		public:
			virtual ~JarImage() = default;

			virtual void Release() = 0;
	};

#pragma endregion JarImage }


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
		std::shared_ptr<JarShaderModule> vertexShaderModule;
		std::shared_ptr<JarShaderModule> fragmentShaderModule;
		std::string mainFunctionName;

	};

	enum class VertexInputRate {
		PerVertex,
		PerInstance,
	};

	struct BindingDescription {
		uint32_t bindingIndex;
		uint32_t stride;
		VertexInputRate inputRate;
		uint32_t stepRate;
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
		uint32_t bindingIndex;
		uint32_t attributeLocation;
		uint32_t offset;
		VertexFormat vertexFormat;
	};

	struct VertexInput {
		std::vector<BindingDescription> bindingDescriptions;
		std::vector<AttributeDescription> attributeDescriptions;
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
		PixelFormat pixelFormat;
		bool blendingEnabled;
		BlendFactor sourceRgbBlendFactor;
		BlendFactor destinationRgbBlendFactor;
		BlendOperation rgbBlendOperation;
		BlendFactor sourceAlphaBlendFactor;
		BlendFactor destinationAlphaBlendFactor;
		BlendOperation alphaBlendOperation;
		ColorWriteMask colorWriteMask;
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
		bool depthTestEnable;
		bool depthWriteEnable;
		DepthCompareOperation depthCompareOp;
		bool stencilTestEnable;
		StencilOpState stencilOpState;
	};

	enum class StageFlags {
		VertexShader = 0x1,
		FragmentShader = 0x2,
		GeometryShader = 0x4,
		ComputeShader = 0x8,
	};

	class JarPipelineBuilder {
		public:
			virtual ~JarPipelineBuilder() = default;

			virtual JarPipelineBuilder* SetShaderStage(ShaderStage shaderStage) = 0;

			virtual JarPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) = 0;

			virtual JarPipelineBuilder* SetVertexInput(VertexInput vertexInput) = 0;

			virtual JarPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) = 0;

			virtual JarPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) = 0;

			virtual JarPipelineBuilder*
			BindUniformBuffers(std::vector<std::shared_ptr<JarBuffer>> uniformBuffers, uint32_t binding,
			                   StageFlags stageFlags) = 0;

			virtual JarPipelineBuilder*
			BindImageBuffer(std::shared_ptr<JarImage> imageBuffer, uint32_t binding, StageFlags stageFlags) = 0;

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

			virtual void BindPipeline(std::shared_ptr<JarPipeline> pipeline, uint32_t imageIndex) = 0;

			virtual void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) = 0;

			virtual void BindIndexBuffer(std::shared_ptr<JarBuffer> indexBuffer) = 0;

			virtual void Draw() = 0;

			virtual void DrawIndexed(size_t indexAmount) = 0;

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

	enum class BackendType {
		Vulkan,
		Metal,
	};


	class Backend {
		public:
			virtual ~Backend() = default;

			virtual BackendType GetType() = 0;

			virtual std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* windowHandleProvider) = 0;

			virtual std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& surface) = 0;

			virtual JarShaderModuleBuilder* InitShaderModuleBuilder() = 0;

			virtual JarRenderPassBuilder* InitRenderPassBuilder() = 0;

			virtual JarCommandQueueBuilder* InitCommandQueueBuilder() = 0;

			virtual JarBufferBuilder* InitBufferBuilder() = 0;

			virtual JarImageBuilder* InitImageBuilder() = 0;

			virtual JarPipelineBuilder* InitPipelineBuilder() = 0;
	};

#pragma endregion Backend }
}
#endif //JAREP_IRENDERER_HPP
