//
// Created by Sebastian Borsch on 24.10.23.
//

#include "JarepGraphics.hpp"

namespace Graphics {
	JarepGraphics::JarepGraphics(const std::vector<const char*>& extensionNames) {
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
				{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
				{{0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}},
				{{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}},
		};

		const std::vector<uint16_t> indices = {
				0, 1, 2, 2, 3, 0
		};

		const size_t vertexDataSize = vertices.size() * sizeof(Vertex);

		MemoryProperties memoryProps = MemoryProperties::HostVisible | MemoryProperties::HostCoherent;

		const auto bufferBuilder = backend->InitBufferBuilder();
		bufferBuilder->SetBufferData(vertices.data(), vertexDataSize);
		bufferBuilder->SetMemoryProperties(memoryProps);
		bufferBuilder->SetUsageFlags(BufferUsage::VertexBuffer);
		vertexBuffer = bufferBuilder->Build(device);

		const size_t indexBufferSize = sizeof(indices[0]) * indices.size();


		vertexShaderModule = createShaderModule(VertexShader, "triangle_vert");
		fragmentShaderModule = createShaderModule(FragmentShader, "triangle_frag");

		ColorAttachment colorAttachment;
		colorAttachment.loadOp = LoadOp::Clear;
		colorAttachment.storeOp = StoreOp::Store;
		colorAttachment.clearColor = ClearColor(0, 0, 0, 0);
		colorAttachment.imageFormat = B8G8R8A8_UNORM;

		JarRenderPassBuilder* rpBuilder = backend->InitRenderPassBuilder();
		rpBuilder->AddColorAttachment(colorAttachment);
		renderPass = rpBuilder->Build(device, surface);
		delete rpBuilder;

		ShaderStage shaderStage{};
		shaderStage.vertexShaderModule = vertexShaderModule;
		shaderStage.fragmentShaderModule = fragmentShaderModule;
		shaderStage.mainFunctionName = "main";


		std::vector attributeDescriptions = {AttributeDescription{}, AttributeDescription{}};
		attributeDescriptions[0].vertexFormat = VertexFormat::Float3;
		attributeDescriptions[0].offset = 0;
		attributeDescriptions[0].bindingIndex = 0;
		attributeDescriptions[0].attributeLocation = 0;

		attributeDescriptions[1].vertexFormat = VertexFormat::Float3;
		attributeDescriptions[1].offset = sizeof(float) * 3;
		attributeDescriptions[1].bindingIndex = 0;
		attributeDescriptions[1].attributeLocation = 1;

		std::vector bindingDescriptions = {BindingDescription{}};
		bindingDescriptions[0].bindingIndex = 0;
		bindingDescriptions[0].inputRate = VertexInputRate::PerVertex;
		bindingDescriptions[0].stride = sizeof(float) * 6;
		bindingDescriptions[0].stepRate = 1;

		VertexInput vertexInput{};
		vertexInput.attributeDescriptions = attributeDescriptions;
		vertexInput.bindingDescriptions = bindingDescriptions;

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.pixelFormat = PixelFormat::BGRA8_UNORM;
		colorBlendAttachment.sourceRgbBlendFactor = BlendFactor::One;
		colorBlendAttachment.destinationRgbBlendFactor = BlendFactor::Zero;
		colorBlendAttachment.rgbBlendOperation = BlendOperation::Add;
		colorBlendAttachment.blendingEnabled = false;
		colorBlendAttachment.sourceAlphaBlendFactor = BlendFactor::One;
		colorBlendAttachment.destinationAlphaBlendFactor = BlendFactor::Zero;
		colorBlendAttachment.alphaBlendOperation = BlendOperation::Add;
		colorBlendAttachment.colorWriteMask = ColorWriteMask::All;


		JarPipelineBuilder* pipelineBuilder = backend->InitPipelineBuilder();
		pipelineBuilder->
				SetShaderStage(shaderStage)->
				SetRenderPass(renderPass)->
				SetVertexInput(vertexInput)->
				SetInputAssemblyTopology(InputAssemblyTopology::TriangleList)->
				SetMultisamplingCount(1)->SetColorBlendAttachments(colorBlendAttachment);
		pipeline = pipelineBuilder->Build(device);
		delete pipelineBuilder;
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

		surface->ReleaseSwapchain();

		pipeline->Release();
		vertexShaderModule->Release();
		fragmentShaderModule->Release();

		vertexBuffer->Release();

		queue->Release();
		device->Release();


		std::cout << "Shutdown renderer" << std::endl;
	}

	std::shared_ptr<JarShaderModule> JarepGraphics::createShaderModule(const ShaderType shaderType,
	                                                                   const std::string& shaderName) const {
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
