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
		mvp.model = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		auto bufferBuilder = backend->InitBufferBuilder()->SetUsageFlags(
				BufferUsage::UniformBuffer)->SetMemoryProperties(
				MemoryProperties::HostVisible | MemoryProperties::HostCoherent)->SetBufferData(
				&mvp, sizeof(Internal::JarModelViewProjection));
		for (int i = 0; i < surface->GetSwapchainImageAmount(); ++i) {
			uniformBuffers.push_back(bufferBuilder->Build(device));
		}

		auto image = backend->InitImageBuilder()->EnableMipMaps(true)->SetPixelFormat(
				PixelFormat::BGRA8_UNORM)->SetImagePath(
				"../../resources/uv_texture.jpg")->Build(device);
		images.push_back(image);

		vertexShaderModule = createShaderModule(VertexShader, "triangle_vert");
		fragmentShaderModule = createShaderModule(FragmentShader, "triangle_frag");

		ColorAttachment colorAttachment;
		colorAttachment.loadOp = LoadOp::Clear;
		colorAttachment.storeOp = StoreOp::Store;
		colorAttachment.clearColor = ClearColor(0, 0, 0, 0);
		colorAttachment.imageFormat = B8G8R8A8_UNORM;

		StencilAttachment stencilAttachment = {};
		stencilAttachment.StencilLoadOp = LoadOp::DontCare;
		stencilAttachment.StencilStoreOp = StoreOp::DontCare;
		stencilAttachment.StencilClearValue = 0;

		DepthAttachment depthStencilAttachment;
		depthStencilAttachment.Format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.DepthLoadOp = LoadOp::Clear,
		depthStencilAttachment.DepthStoreOp = StoreOp::DontCare,
		depthStencilAttachment.DepthClearValue = 1.0f;
		depthStencilAttachment.Stencil = std::make_optional(stencilAttachment);


		JarRenderPassBuilder* rpBuilder = backend->InitRenderPassBuilder();
		rpBuilder->AddColorAttachment(colorAttachment);
		rpBuilder->AddDepthStencilAttachment(depthStencilAttachment);
		renderPass = rpBuilder->Build(device, surface);
		delete rpBuilder;

		ShaderStage shaderStage{};
		shaderStage.vertexShaderModule = vertexShaderModule;
		shaderStage.fragmentShaderModule = fragmentShaderModule;
		shaderStage.mainFunctionName = "main";

		std::vector attributeDescriptions = {AttributeDescription{}, AttributeDescription{}, AttributeDescription{}};
		attributeDescriptions[0].vertexFormat = VertexFormat::Float3;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		attributeDescriptions[0].bindingIndex = 0;
		attributeDescriptions[0].attributeLocation = 0;

		attributeDescriptions[1].vertexFormat = VertexFormat::Float3;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		attributeDescriptions[1].bindingIndex = 0;
		attributeDescriptions[1].attributeLocation = 1;

		attributeDescriptions[2].bindingIndex = 0;
		attributeDescriptions[2].attributeLocation = 2;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
		attributeDescriptions[2].vertexFormat = VertexFormat::Float2;

		std::vector bindingDescriptions = {BindingDescription{}};
		bindingDescriptions[0].bindingIndex = 0;
		bindingDescriptions[0].inputRate = VertexInputRate::PerVertex;
		bindingDescriptions[0].stride = sizeof(Vertex);
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

		DepthStencilState depthStencilState{};
		depthStencilState.depthTestEnable = true;
		depthStencilState.depthWriteEnable = true;
		depthStencilState.depthCompareOp = DepthCompareOperation::Less;
		depthStencilState.stencilTestEnable = false;
		depthStencilState.stencilOpState = {};


		JarPipelineBuilder* pipelineBuilder = backend->InitPipelineBuilder();
		pipelineBuilder->
				SetShaderStage(shaderStage)->
				SetRenderPass(renderPass)->
				SetVertexInput(vertexInput)->
				SetInputAssemblyTopology(InputAssemblyTopology::TriangleList)->
				SetMultisamplingCount(8)->
				BindUniformBuffers(uniformBuffers, 1, StageFlags::VertexShader)->
				BindImageBuffer(images[0], 2, StageFlags::FragmentShader)->
				SetColorBlendAttachments(colorBlendAttachment)->
				SetDepthStencilState(depthStencilState);
		pipeline = pipelineBuilder->Build(device);
		delete pipelineBuilder;

	}

	void JarepGraphics::Resize(uint32_t width, uint32_t height) {
		surface->RecreateSurface(width, height);
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
		if (!commandBuffer->StartRecording(surface, renderPass))
			return;

		commandBuffer->BindPipeline(pipeline, frameCounter);


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

		for (auto& image: images) {
			image->Release();
		}

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
		mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.projection = glm::perspectiveRH_NO(glm::radians(45.0f), surfaceExtent.Width / surfaceExtent.Height, 0.1f,
		                                       100.0f);

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
