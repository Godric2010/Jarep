//
// Created by Sebastian Borsch on 30.03.24.
//

#include "JarRenderStep.hpp"

namespace Graphics {

	namespace Internal {
		JarRenderStep::JarRenderStep(std::unique_ptr<JarRenderStepDescriptor> desc, std::shared_ptr<Backend> backend,
		                             std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface,
		                             std::vector<std::shared_ptr<JarDescriptor>> descriptors)
				: descriptor(std::move(desc)), descriptors(descriptors) {
			BuildShaderModules(backend, device);
			BuildRenderPass(backend, surface, device);
			BuildPipeline(backend, device, descriptors);
		}

		void JarRenderStep::Release() {
			for (auto descriptor: descriptors) {
				descriptor->Release();
			}
			pipeline->Release();
			shaderStage.vertexShaderModule->Release();
			shaderStage.fragmentShaderModule->Release();
		}

#pragma region ShaderCreation{

		void JarRenderStep::BuildShaderModules(std::shared_ptr<Backend> backend, std::shared_ptr<JarDevice> device) {
			auto vertexShaderModule = GetShaderModule(descriptor->m_vertexShaderName, ShaderType::VertexShader, backend,
			                                          device);
			auto fragmentShaderModule = GetShaderModule(descriptor->m_fragmentShaderName, ShaderType::FragmentShader,
			                                            backend, device);

			ShaderStage stage = {};
			stage.vertexShaderModule = vertexShaderModule;
			stage.fragmentShaderModule = fragmentShaderModule;
			stage.mainFunctionName = "main";

			shaderStage = stage;

		}

		std::shared_ptr<JarShaderModule>
		JarRenderStep::GetShaderModule(const std::string& shaderName, ShaderType type,
		                               std::shared_ptr<Backend> backend,
		                               std::shared_ptr<JarDevice> device) {

			auto shaderFileType = "";
			if (backend->GetType() == BackendType::Vulkan) {
				shaderFileType = ".spv";
			} else {
				shaderFileType = ".metal";
			}

			const auto shaderDir = "shaders/";
			const std::string shaderFilePath = shaderDir + shaderName + shaderFileType;
			const std::string shaderCodeString = readFile(shaderFilePath);

			const auto shaderModuleBuilder = backend->InitShaderModuleBuilder();
			shaderModuleBuilder->SetShader(shaderCodeString);
			shaderModuleBuilder->SetShaderType(type);
			auto shaderModule = shaderModuleBuilder->Build(device);
			delete shaderModuleBuilder;
			return shaderModule;
		}

		std::string JarRenderStep::readFile(const std::string& filename) {
			std::ifstream file(filename, std::ios::ate | std::ios::binary);

			if (!std::filesystem::exists(filename)) {
				throw std::runtime_error("File does not exist: " + filename);
			}

			if (!file.is_open()) {
				throw std::runtime_error("Failed to open file: " + filename);
			}

			auto fileSize = (size_t) file.tellg();
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();
			std::string bufferString(buffer.begin(), buffer.end());
			return bufferString;
		}

#pragma endregion ShaderCreation }

#pragma region RenderPassCreation{

		void
		JarRenderStep::BuildRenderPass(const std::shared_ptr<Backend>& backend, std::shared_ptr<JarSurface> surface,
		                               std::shared_ptr<JarDevice> device) {
			ColorAttachment colorAttachment;
			colorAttachment.loadOp = LoadOp::Clear;
			colorAttachment.storeOp = StoreOp::Store;
			colorAttachment.clearColor = ClearColor(0, 0, 0, 0);
			colorAttachment.imageFormat = B8G8R8A8_UNORM;

			JarRenderPassBuilder* rpBuilder = backend->InitRenderPassBuilder();
			rpBuilder->AddColorAttachment(colorAttachment);

			if (descriptor->m_depthTestEnabled) {
				std::optional<StencilAttachment> stencilAttachment = std::nullopt;

				if (descriptor->m_stencilTestEnabled) {

					StencilAttachment stencil = {};
					stencil.StencilLoadOp = LoadOp::DontCare;
					stencil.StencilStoreOp = StoreOp::DontCare;
					stencil.StencilClearValue = 0;
					stencilAttachment = std::make_optional(stencil);
				}

				DepthAttachment depthStencilAttachment;
				depthStencilAttachment.Format = ImageFormat::D32_SFLOAT;
				depthStencilAttachment.DepthLoadOp = LoadOp::Clear,
				depthStencilAttachment.DepthStoreOp = StoreOp::DontCare,
				depthStencilAttachment.DepthClearValue = 1.0f;
				depthStencilAttachment.Stencil = stencilAttachment;

				rpBuilder->AddDepthStencilAttachment(depthStencilAttachment);
			}
			renderPass = rpBuilder->Build(device, surface);
			delete rpBuilder;
		}

#pragma endregion RenderPassCreation };

#pragma region PipelineCreation{

		void JarRenderStep::BuildPipeline(const std::shared_ptr<Backend>& backend, std::shared_ptr<JarDevice> device,
		                                  std::vector<std::shared_ptr<JarDescriptor>> descriptors) {

			std::vector<std::shared_ptr<JarDescriptorLayout>> descriptorLayouts = std::vector<std::shared_ptr<JarDescriptorLayout>>();
			for (const auto& descriptor: descriptors) {
				descriptorLayouts.push_back(descriptor->GetDescriptorLayout());
			}


			VertexInput vertexInput{};
			vertexInput.attributeDescriptions = Vertex::GetAttributeDescriptions();
			vertexInput.bindingDescriptions = Vertex::GetBindingDescriptions();

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
					SetMultisamplingCount(descriptor->m_multisamplingCount)->
					BindDescriptorLayouts(descriptorLayouts)->
					SetColorBlendAttachments(colorBlendAttachment)->
					SetDepthStencilState(depthStencilState);
			pipeline = pipelineBuilder->Build(device);
			delete pipelineBuilder;

		}

#pragma endregion PipelineCreation };

	}

} // Graphics