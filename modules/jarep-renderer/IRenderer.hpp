//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

#include <complex.h>

#include "NativeWindowHandleProvider.hpp"
#include <memory>
#include <string>

namespace Graphics {
	class JarDevice;
	class JarShaderModule;
	class JarRenderPass;

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
			ImageFormat ImageFormat;
			LoadOp LoadOp;
			StoreOp StoreOp;
			ClearColor ClearColor;

			ColorAttachment() : ClearColor(0, 0, 0, 0), ImageFormat(ImageFormat::B8G8R8A8_UNORM), LoadOp(LoadOp::Clear),
			                    StoreOp(StoreOp::DontCare) {
			}
	};

	//	struct DepthStencilAttachment{
	//		public:
	//			ImageFormat ImageFormat;
	//			StoreOp StencilStoreOp;
	//			LoadOp StencilLoadOp;
	//			float DepthClearValue;
	//			uint32_t StencilClearValue;
	//	};

	class JarRenderPassBuilder {
		public:
			virtual ~JarRenderPassBuilder() = default;

			virtual JarRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) = 0;

			virtual std::shared_ptr<JarRenderPass> Build(std::shared_ptr<JarDevice> device) = 0;
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


	class JarBuffer {
		public:
			virtual ~JarBuffer() = default;
	};

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

	class JarPipeline {
		public:
			virtual ~JarPipeline() = default;

			virtual void Release() = 0;

			virtual std::shared_ptr<JarRenderPass> GetRenderPass() = 0;
	};

	class JarCommandBuffer {
		public:
			virtual ~JarCommandBuffer() = default;

			virtual void
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) = 0;

			virtual void EndRecording() = 0;

			virtual void BindPipeline(std::shared_ptr<JarPipeline> pipeline) = 0;

			virtual void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) = 0;

			virtual void Draw() = 0;

			virtual void Present(std::shared_ptr<JarSurface>&surface, std::shared_ptr<JarDevice> device) = 0;
	};


	class JarCommandQueue {
		public:
			virtual ~JarCommandQueue() = default;

			virtual JarCommandBuffer* getNextCommandBuffer() = 0;

			virtual void Release(std::shared_ptr<JarDevice> device) = 0;
	};


	class JarDevice {
		public:
			virtual ~JarDevice() = default;

			virtual void Release() = 0;

			virtual std::shared_ptr<JarCommandQueue> CreateCommandQueue() = 0;

			virtual std::shared_ptr<JarBuffer> CreateBuffer(size_t bufferSize, const void* data) = 0;

			virtual std::shared_ptr<JarPipeline> CreatePipeline(std::shared_ptr<JarShaderModule> vertexModule,
			                                                    std::shared_ptr<JarShaderModule> fragmentModule,
			                                                    std::shared_ptr<JarRenderPass> renderPass) = 0;
	};

#pragma region Backend{

	class Backend {
		public:
			virtual ~Backend() = default;

			virtual std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* windowHandleProvider) = 0;

			virtual std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>&surface) = 0;

			virtual JarShaderModuleBuilder* InitShaderModuleBuilder() = 0;

			virtual JarRenderPassBuilder* InitRenderPassBuilder() = 0;
	};

#pragma endregion Backend }
}
#endif //JAREP_IRENDERER_HPP
