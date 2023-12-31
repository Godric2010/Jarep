//
// Created by Sebastian Borsch on 24.10.23.
//

#include "JarepGraphics.hpp"

namespace Graphics {
	JarepGraphics::JarepGraphics(const std::vector<const char *>&extensionNames) {
		extensions = extensionNames;
#if defined(__APPLE__) && defined(__MACH__)
		backend = std::make_shared<Metal::MetalBackend>(Metal::MetalBackend());
		shaderFileType = ".metal";
		std::cout << "Using metal renderer!" << std::endl;
#else
		backend = std::make_shared<Vulkan::VulkanBackend>(Vulkan::VulkanBackend(extensionNames));
		shaderFileType = ".spv";
		std::cout << "Using vulkan renderer!" << std::endl;
#endif
	}

	void JarepGraphics::Initialize(NativeWindowHandleProvider* nativeWindowHandle) {
		surface = backend->CreateSurface(nativeWindowHandle);
		device = backend->CreateDevice(surface);

		const auto commandQueueBuilder = backend->InitCommandQueueBuilder();
		queue = commandQueueBuilder->Build(device);

		const std::vector<Vertex> vertices = {
			{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};

		const size_t vertexDataSize = vertices.size() * sizeof(Vertex);
		vertexBuffer = device->CreateBuffer(vertexDataSize, vertices.data());
		vertexShaderModule = createShaderModule(VertexShader, "triangle_vert");
		fragmentShaderModule = createShaderModule(FragmentShader, "triangle_frag");

		ColorAttachment colorAttachment;
		colorAttachment.LoadOp = LoadOp::Clear;
		colorAttachment.StoreOp = StoreOp::Store;
		colorAttachment.ClearColor = ClearColor(0, 0, 0, 0);
		colorAttachment.ImageFormat = ImageFormat::B8G8R8A8_UNORM;

		JarRenderPassBuilder* rpBuilder = backend->InitRenderPassBuilder();
		rpBuilder->AddColorAttachment(colorAttachment);
		renderPass = rpBuilder->Build(device);
		delete rpBuilder;

		pipeline = device->CreatePipeline(vertexShaderModule, fragmentShaderModule, renderPass);
	}

	void JarepGraphics::Render() {
		const auto commandBuffer = queue->getNextCommandBuffer();
		commandBuffer->StartRecording(surface, renderPass);

		commandBuffer->BindPipeline(pipeline);
		commandBuffer->BindVertexBuffer(vertexBuffer);
		commandBuffer->Draw();

		commandBuffer->EndRecording();
		commandBuffer->Present(surface, device);
	}

	void JarepGraphics::Shutdown() {
		pipeline->Release();
		vertexShaderModule->Release();
		fragmentShaderModule->Release();

		queue->Release();
		device->Release();

		std::cout << "Shutdown renderer" << std::endl;
	}

	std::shared_ptr<JarShaderModule> JarepGraphics::createShaderModule(const ShaderType shaderType,
	                                                                   const std::string&shaderName) const {
		const auto shaderDir = "shaders/";
		const std::string shaderFilePath = shaderDir + shaderName + shaderFileType;
		const std::string shaderCodeString = readFile(shaderFilePath);

		const auto shaderModuleBuilder = backend->InitShaderModuleBuilder();
		shaderModuleBuilder->SetShader(shaderCodeString);
		shaderModuleBuilder->SetShaderType(shaderType);
		auto shaderModule = shaderModuleBuilder->Build(device);
		delete shaderModuleBuilder;

		return shaderModule;
	}
}
