//
// Created by Sebastian Borsch on 30.03.24.
//

#include "JarRenderStep.hpp"

namespace Graphics {

	namespace Internal {
		JarRenderStep::JarRenderStep(std::unique_ptr<JarRenderStepDescriptor> desc, std::shared_ptr<Backend> backend,
		                             std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface,
		                             std::vector<std::shared_ptr<JarDescriptor>> descriptors)
				: renderStepDescriptor(std::move(desc)), descriptors(descriptors) {
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
			auto vertexShaderModule = GetShaderModule(renderStepDescriptor->m_vertexShaderName,
			                                          ShaderType::VertexShader, backend,
			                                          device);
			auto fragmentShaderModule = GetShaderModule(renderStepDescriptor->m_fragmentShaderName,
			                                            ShaderType::FragmentShader,
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
			colorAttachment.imageFormat = PixelFormat::BGRA8Unorm;

			JarRenderPassBuilder* rpBuilder = backend->InitRenderPassBuilder();
			rpBuilder->AddColorAttachment(colorAttachment);

			uint32_t maxMultisamplingCount = device->GetMaxUsableSampleCount();
			uint32_t multisamplingCount = renderStepDescriptor->m_multisamplingCount;
			if (multisamplingCount > maxMultisamplingCount) {
				multisamplingCount = maxMultisamplingCount;
			}
			rpBuilder->SetMultisamplingCount(multisamplingCount);

			if (renderStepDescriptor->m_depthTestEnabled) {

				PixelFormat depthFormat = PixelFormat::Depth32Float;
				if (!device->IsFormatSupported(depthFormat))
					throw std::runtime_error("The selected depth format is not supported by the device!");

				std::optional<StencilAttachment> stencilAttachment = std::nullopt;
				DepthAttachment depthStencilAttachment;
				depthStencilAttachment.Format = depthFormat;
				depthStencilAttachment.DepthLoadOp = LoadOp::Clear,
				depthStencilAttachment.DepthStoreOp = StoreOp::DontCare,
				depthStencilAttachment.DepthClearValue = 1.0f;
				depthStencilAttachment.Stencil = stencilAttachment;

				if (renderStepDescriptor->m_stencilTestEnabled) {

					PixelFormat depthStencilFormat = PixelFormat::Depth32FloatStencil8;
					if (!device->IsFormatSupported(depthStencilFormat))
						throw std::runtime_error("The selected depth stencil format is not supported by the device!");

					StencilAttachment stencil = {};
					stencil.StencilLoadOp = LoadOp::DontCare;
					stencil.StencilStoreOp = StoreOp::DontCare;
					stencil.StencilClearValue = 0;
					stencilAttachment = std::make_optional(stencil);

					depthStencilAttachment.Stencil = stencilAttachment;
					depthStencilAttachment.Format = depthStencilFormat;
				}

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
			colorBlendAttachment.pixelFormat = PixelFormat::BGRA8Unorm;
			colorBlendAttachment.sourceRgbBlendFactor = BlendFactor::One;
			colorBlendAttachment.destinationRgbBlendFactor = BlendFactor::Zero;
			colorBlendAttachment.rgbBlendOperation = BlendOperation::Add;
			colorBlendAttachment.blendingEnabled = false;
			colorBlendAttachment.sourceAlphaBlendFactor = BlendFactor::One;
			colorBlendAttachment.destinationAlphaBlendFactor = BlendFactor::Zero;
			colorBlendAttachment.alphaBlendOperation = BlendOperation::Add;
			colorBlendAttachment.colorWriteMask = ColorWriteMask::All;

			bool depthTestEnabled = renderStepDescriptor->m_depthTestEnabled;
			bool stencilTestEnabled = renderStepDescriptor->m_stencilTestEnabled;
			DepthStencilState depthStencilState{};
			depthStencilState.depthTestEnable = depthTestEnabled;
			depthStencilState.depthWriteEnable = depthTestEnabled;
			depthStencilState.depthCompareOp = CompareOperation::Less;
			depthStencilState.stencilTestEnable = stencilTestEnabled;
			depthStencilState.stencilFailOp = StencilOpState::Keep;
			depthStencilState.stencilPassOp = StencilOpState::Replace;
			depthStencilState.stencilDepthFailOp = StencilOpState::Keep;
			depthStencilState.stencilCompareOp = CompareOperation::AllTime;

			uint32_t maxMultisamplingCount = device->GetMaxUsableSampleCount();
			uint32_t multisamplingCount = renderStepDescriptor->m_multisamplingCount;
			if (multisamplingCount > maxMultisamplingCount) {
				multisamplingCount = maxMultisamplingCount;
			}

			JarPipelineBuilder* pipelineBuilder = backend->InitPipelineBuilder();
			if (depthTestEnabled)
				pipelineBuilder->SetDepthStencilState(depthStencilState);

			pipelineBuilder->
					SetShaderStage(shaderStage)->
					SetRenderPass(renderPass)->
					SetVertexInput(vertexInput)->
					SetInputAssemblyTopology(InputAssemblyTopology::TriangleList)->
					SetMultisamplingCount(multisamplingCount)->
					BindDescriptorLayouts(descriptorLayouts)->
					SetColorBlendAttachments(colorBlendAttachment);
			pipeline = pipelineBuilder->Build(device);
			delete pipelineBuilder;

		}

#pragma endregion PipelineCreation };

	}

} // Graphics