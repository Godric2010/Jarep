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

		Internal::JarModelViewProjection mvp{};
		auto bufferBuilder = backend->InitBufferBuilder()->SetUsageFlags(
				BufferUsage::UniformBuffer)->SetMemoryProperties(
				MemoryProperties::HostVisible | MemoryProperties::HostCoherent)->SetBufferData(
				&mvp, sizeof(Internal::JarModelViewProjection));
		for (int i = 0; i < surface->GetSwapchainImageAmount(); ++i) {
			uniformBuffers.push_back(bufferBuilder->Build(device));
		}

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
				SetMultisamplingCount(1)->
				SetUniformBuffers(uniformBuffers)->
				SetColorBlendAttachments(colorBlendAttachment);
		pipeline = pipelineBuilder->Build(device);
		delete pipelineBuilder;
	}

	void JarepGraphics::AddMesh(Mesh& mesh) {

		const size_t vertexDataSize = mesh.getVertices().size() * sizeof(Vertex);

		const auto vertexBufferBuilder = backend->InitBufferBuilder();
		vertexBufferBuilder->SetBufferData(mesh.getVertices().data(), vertexDataSize);
		vertexBufferBuilder->SetMemoryProperties(MemoryProperties::DeviceLocal);
		vertexBufferBuilder->SetUsageFlags(BufferUsage::VertexBuffer);
		std::shared_ptr<JarBuffer> vertexBuffer = vertexBufferBuilder->Build(device);

		const size_t indexBufferSize = sizeof(mesh.getIndices()[0]) * mesh.getIndices().size();
		const auto indexBufferBuilder = backend->
				InitBufferBuilder()->
				SetBufferData(mesh.getIndices().data(), indexBufferSize)->
				SetMemoryProperties(MemoryProperties::DeviceLocal)->
				SetUsageFlags(BufferUsage::IndexBuffer);
		std::shared_ptr<JarBuffer> indexBuffer = indexBufferBuilder->Build(device);

		meshes.push_back(Internal::JarMesh(mesh, vertexBuffer, indexBuffer));
	}

	void JarepGraphics::Render() {


		prepareModelViewProjectionForFrame();

		const auto commandBuffer = queue->getNextCommandBuffer();
		commandBuffer->StartRecording(surface, renderPass);

		commandBuffer->BindPipeline(pipeline);
		commandBuffer->BindUniformBuffer(uniformBuffers[frameCounter]);

		for (auto& mesh: meshes) {
			commandBuffer->BindVertexBuffer(mesh.getVertexBuffer());
			commandBuffer->BindIndexBuffer(mesh.getIndexBuffer());
			commandBuffer->DrawIndexed(mesh.getIndexLength());
		}

		commandBuffer->EndRecording();
		commandBuffer->Present(surface, device);
		frameCounter = (frameCounter + 1) % surface->GetSwapchainImageAmount();
	}

	void JarepGraphics::Shutdown() {

		surface->ReleaseSwapchain();

		for (auto& uniformBuffer: uniformBuffers) {
			uniformBuffer->Release();
		}

		pipeline->Release();
		vertexShaderModule->Release();
		fragmentShaderModule->Release();

		for (auto& mesh: meshes) {
			mesh.Destroy();
		}

		queue->Release();
		device->Release();


		std::cout << "Shutdown renderer" << std::endl;
	}

	void JarepGraphics::prepareModelViewProjectionForFrame() {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		auto surfaceExtent = surface->GetSurfaceExtent();

		Internal::JarModelViewProjection mvp{};
		mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		mvp.projection = glm::perspective(glm::radians(45.0f), surfaceExtent.Width / surfaceExtent.Height, 0.1f,
		                                  100.0f);

		mvp.projection[1][1] *= -1;

		uniformBuffers[frameCounter]->Update(&mvp, sizeof(Internal::JarModelViewProjection));
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
