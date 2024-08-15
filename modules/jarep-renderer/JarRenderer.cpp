//
// Created by Sebastian Borsch on 24.10.23.
//

#include "JarRenderer.hpp"
#include "JarGraphicsApi/include/JarBackend.hpp"


namespace Graphics {
	JarRenderer::JarRenderer(const std::vector<const char*>& extensionNames) {
		//		backend = Graphics::CreateBackend(extensionNames);
		auto backend = JarBackend::Create(extensionNames);
	}

	void JarRenderer::Initialize(NativeWindowHandleProvider* nativeWindowHandle, uint32_t resolutionX, uint32_t resolutionY) {
		//		surface = backend->CreateSurface(nativeWindowHandle);
		//		device = backend->CreateDevice(surface);
		//
		//		renderTarget = backend->InitRenderTargetBuilder()
		//		                       ->SetRenderTargetType(RenderTargetType::Texture)
		//		                       ->SetImageFormat(PixelFormat::BGRA8Unorm)
		//		                       ->SetResolution(resolutionX, resolutionY)
		//		                       ->SetMultisamplingCount(1)
		//		                       ->Build();
		//
		//		const auto commandQueueBuilder = backend->InitCommandQueueBuilder();
		//		queue = commandQueueBuilder->Build(device);
		//
		//		CreateDepthResources(PixelFormat::Depth32Float);
		//		CreateMultisamplingResources(PixelFormat::BGRA8Unorm);
		//
		//		Internal::JarModelViewProjection mvp{};
		//		//		mvp.model = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//		auto bufferBuilder = backend->InitBufferBuilder()->SetUsageFlags(
		//		                                                         BufferUsage::UniformBuffer)
		//		                             ->SetMemoryProperties(
		//		                                     MemoryProperties::HostVisible | MemoryProperties::HostCoherent)
		//		                             ->SetBufferData(
		//		                                     &mvp, sizeof(Internal::JarModelViewProjection));
		//		for (int i = 0; i < surface->GetSwapchainImageAmount(); ++i) {
		//			uniformBuffers.push_back(bufferBuilder->Build(device));
		//		}
		//
		//		descriptors = std::vector<std::shared_ptr<JarDescriptor>>();
		//		auto uboDescriptor = backend->InitDescriptorBuilder()->SetBinding(1)->SetStageFlags(
		//		                                                                            StageFlags::VertexShader)
		//		                             ->BuildUniformBufferDescriptor(device, uniformBuffers);
		//		descriptors.push_back(uboDescriptor);
		//
		//		auto image = backend->InitImageBuilder()->EnableMipMaps(true)->SetPixelFormat(
		//		                                                                     PixelFormat::BGRA8Unorm)
		//		                     ->SetImagePath(
		//		                             "../../resources/uv_texture.jpg")
		//		                     ->Build(device);
		//		images.push_back(image);
		//		auto imageDescriptor = backend->InitDescriptorBuilder()->SetBinding(2)->SetStageFlags(
		//		                                                                              StageFlags::FragmentShader)
		//		                               ->BuildImageBufferDescriptor(device, image);
		//		descriptors.push_back(imageDescriptor);
		//
		//		std::vector<std::shared_ptr<JarDescriptorLayout>> descriptorLayouts = std::vector<std::shared_ptr<JarDescriptorLayout>>();
		//		for (const auto& descriptor: descriptors) {
		//			descriptorLayouts.push_back(descriptor->GetDescriptorLayout());
		//		}
	}

	void JarRenderer::ResizeSurface(uint32_t width, uint32_t height) {
		//		surface->RecreateSurface(width, height);
	}

	void JarRenderer::ChangeResolution(uint32_t resX, uint32_t resY) {
		//
		//		renderTarget = backend->InitRenderTargetBuilder()->SetRenderTargetType(RenderTargetType::Texture)->SetResolution(resX, resY)->SetMultisamplingCount(64)->SetImageFormat(PixelFormat::BGRA8Unorm)->Build();
		//		CreateMultisamplingResources(PixelFormat::BGRA8Unorm);
		//		CreateDepthResources(PixelFormat::Depth32Float);
		//		for (auto renderStep: renderSteps) {
		//			renderStep->ResizeFramebuffer(backend, device, renderTarget, m_multisamplingBuffer, m_depthBuffer.value());
		//		}
	}

	void JarRenderer::AddRenderStep(std::unique_ptr<JarRenderStepDescriptor> renderStepBuilder) {
		//		auto renderStep = std::make_shared<Internal::JarRenderStep>(std::move(renderStepBuilder), backend, device,
		//		                                                            renderTarget, surface, descriptors,
		//		                                                            m_multisamplingBuffer, m_depthBuffer.value());
		//		renderSteps.push_back(renderStep);
	}

	void JarRenderer::AddMesh(Mesh& mesh) {

		//		const size_t vertexDataSize = mesh.getVertices().size() * sizeof(Vertex);
		//
		//		const auto vertexBufferBuilder = backend->InitBufferBuilder();
		//		vertexBufferBuilder->SetBufferData(mesh.getVertices().data(), vertexDataSize);
		//		vertexBufferBuilder->SetMemoryProperties(MemoryProperties::DeviceLocal);
		//		vertexBufferBuilder->SetUsageFlags(BufferUsage::VertexBuffer);
		//		std::shared_ptr<JarBuffer> vertexBuffer = vertexBufferBuilder->Build(device);
		//
		//		const size_t indexBufferSize = sizeof(mesh.getIndices()[0]) * mesh.getIndices().size();
		//		const auto indexBufferBuilder = backend->InitBufferBuilder()->SetBufferData(mesh.getIndices().data(), indexBufferSize)->SetMemoryProperties(MemoryProperties::DeviceLocal)->SetUsageFlags(BufferUsage::IndexBuffer);
		//		std::shared_ptr<JarBuffer> indexBuffer = indexBufferBuilder->Build(device);
		//		meshes.emplace_back(mesh, vertexBuffer, indexBuffer);
	}

	void JarRenderer::Render() {

		//		Viewport viewport{};
		//		viewport.x = 0;
		//		viewport.y = 0;
		//		viewport.width = static_cast<float>(renderTarget->GetResolutionWidth());
		//		viewport.height = static_cast<float>(renderTarget->GetResolutionHeight());
		//		viewport.minDepth = 0.0f;
		//		viewport.maxDepth = 1.0f;
		//
		//		Scissor scissor{};
		//		scissor.x = 0;
		//		scissor.y = 0;
		//		scissor.width = renderTarget->GetResolutionWidth();
		//		scissor.height = renderTarget->GetResolutionHeight();
		//
		//		DepthBias depthBias{};
		//		depthBias.DepthBiasClamp = 0.0f;
		//		depthBias.DepthBiasConstantFactor = 0.0f;
		//		depthBias.DepthBiasSlopeFactor = 0.0f;
		//
		//		PrepareModelViewProjectionForFrame();
		//
		//		const auto commandBuffer = queue->getNextCommandBuffer();
		//
		//		std::vector<std::shared_ptr<JarFramebuffer>> renderedFramebuffers;
		//		for (auto& renderStep: renderSteps) {
		//			std::shared_ptr<JarFramebuffer> framebuffer = renderStep->GetFramebuffer();
		//			if (!commandBuffer->StartRecording(framebuffer, renderStep->GetRenderPass()))
		//				return;
		//
		//			commandBuffer->SetViewport(viewport);
		//			commandBuffer->SetScissor(scissor);
		//			commandBuffer->SetDepthBias(depthBias);
		//			commandBuffer->BindPipeline(renderStep->GetPipeline(), m_frameCounter);
		//			commandBuffer->BindDescriptors(renderStep->GetDescriptors());
		//
		//
		//			for (auto& mesh: meshes) {
		//				commandBuffer->BindVertexBuffer(mesh.getVertexBuffer());
		//				commandBuffer->BindIndexBuffer(mesh.getIndexBuffer());
		//				commandBuffer->DrawIndexed(mesh.getIndexLength());
		//			}
		//
		//			commandBuffer->EndRecording();
		//			renderedFramebuffers.push_back(framebuffer);
		//		}
		//		commandBuffer->BlitFramebuffersToSurface(surface, renderedFramebuffers);
		//		commandBuffer->Present(surface, device);
		//		m_frameCounter = (m_frameCounter + 1) % surface->GetSwapchainImageAmount();
	}

	void JarRenderer::Shutdown() {

		//		surface->ReleaseSwapchain();
		//
		//		for (auto& image: images) {
		//			image->Release();
		//		}
		//
		//		for (auto& uniformBuffer: uniformBuffers) {
		//			uniformBuffer->Release();
		//		}
		//
		//		for (auto& renderStep: renderSteps) {
		//			renderStep->Release();
		//		}
		//
		//		for (auto& mesh: meshes) {
		//			mesh.Destroy();
		//		}
		//
		//		queue->Release();
		//		device->Release();
		//

		std::cout << "Shutdown renderer" << std::endl;
	}

	void JarRenderer::CreateDepthResources(PixelFormat depthFormat) {
		//		auto depthImageBuffer = backend->InitImageBufferBuilder()
		//		                                ->SetImageBufferExtent(renderTarget->GetResolutionWidth(), renderTarget->GetResolutionHeight())
		//		                                ->SetImageFormat(depthFormat)
		//		                                ->SetMipLevels(1)
		//		                                ->SetSampleCount(renderTarget->GetMultisamplingCount())
		//		                                ->SetImageTiling(ImageTiling::Optimal)
		//		                                ->SetImageUsage(ImageUsage::DepthStencilAttachment)
		//		                                ->SetMemoryProperties(MemoryProperties::DeviceLocal)
		//		                                ->SetImageAspect(ImageAspect::Depth)
		//		                                ->Build(backend, device);
		//
		//		m_depthBuffer = std::make_optional(std::move(depthImageBuffer));
	}

	void JarRenderer::CreateMultisamplingResources(PixelFormat multisamplingFormat) {
		//		m_multisamplingBuffer = backend->InitImageBufferBuilder()
		//		                                ->SetImageBufferExtent(renderTarget->GetResolutionWidth(), renderTarget->GetResolutionHeight())
		//		                                ->SetImageFormat(multisamplingFormat)
		//		                                ->SetMipLevels(1)
		//		                                ->SetSampleCount(renderTarget->GetMultisamplingCount())
		//		                                ->SetImageTiling(ImageTiling::Optimal)
		//		                                ->SetImageUsage(ImageUsage::TransientAttachment | ImageUsage::ColorAttachment)
		//		                                ->SetMemoryProperties(MemoryProperties::DeviceLocal)
		//		                                ->SetImageAspect(ImageAspect::Color)
		//		                                ->Build(backend, device);
	}

	void JarRenderer::PrepareModelViewProjectionForFrame() {
		//		static auto startTime = std::chrono::high_resolution_clock::now();
		//		auto currentTime = std::chrono::high_resolution_clock::now();
		//		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//
		//		auto surfaceExtent = surface->GetSurfaceExtent();
		//
		//		Internal::JarModelViewProjection mvp{};
		//		mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//		mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//		mvp.projection = glm::perspectiveRH_NO(glm::radians(45.0f), surfaceExtent.Width / surfaceExtent.Height, 0.1f,
		//		                                       100.0f);
		//
		//		uniformBuffers[m_frameCounter]->Update(&mvp, sizeof(Internal::JarModelViewProjection));
	}
}// namespace Graphics
