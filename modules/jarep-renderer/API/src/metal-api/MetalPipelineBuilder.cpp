//
// Created by Sebastian Borsch on 28.05.24.
//

#include "MetalPipelineBuilder.hpp"

namespace Graphics::Metal{

	MetalPipelineBuilder::~MetalPipelineBuilder() =
	default;

	MetalPipelineBuilder* MetalPipelineBuilder::SetShaderStage(ShaderStage shaderStage) {
		std::string shaderMainFuncName = shaderStage.mainFunctionName + "0";
		NS::String* mainFuncName = NS::String::string(shaderMainFuncName.c_str(),
		                                              NS::StringEncoding::ASCIIStringEncoding);

		const auto metalVertexShaderModule = reinterpret_cast<std::shared_ptr<MetalShaderLibrary>&>(shaderStage.
				vertexShaderModule);
		const auto metalFragmentShaderModule = reinterpret_cast<std::shared_ptr<MetalShaderLibrary>&>(shaderStage.
				fragmentShaderModule);

		m_vertexShaderFunc = metalVertexShaderModule->getLibrary()->newFunction(mainFuncName);
		m_fragmentShaderFunc = metalFragmentShaderModule->getLibrary()->newFunction(mainFuncName);
		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) {
		m_renderPass = renderPass;
		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetVertexInput(VertexInput vertexInput) {
		m_vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

		for (int i = 0; i < vertexInput.attributeDescriptions.size(); ++i) {
			m_vertexDescriptor->attributes()->object(i)->setBufferIndex(
					vertexInput.attributeDescriptions[i].bindingIndex);
			m_vertexDescriptor->attributes()->object(i)->setOffset(vertexInput.attributeDescriptions[i].offset);
			m_vertexDescriptor->attributes()->object(i)->setFormat(
					vertexFormatMap[vertexInput.attributeDescriptions[i].vertexFormat]);
		}

		for (auto& bindingDescriptor: vertexInput.bindingDescriptions) {
			m_vertexDescriptor->layouts()->object(bindingDescriptor.bindingIndex)->setStepFunction(
					vertexInputRateMap[bindingDescriptor.inputRate]);
			m_vertexDescriptor->layouts()->object(bindingDescriptor.bindingIndex)->setStride(
					bindingDescriptor.stride);
			m_vertexDescriptor->layouts()->object(bindingDescriptor.bindingIndex)->setStepRate(
					bindingDescriptor.stepRate);
		}

		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetInputAssemblyTopology(InputAssemblyTopology topology) {
		m_topology = topologyMap[topology];
		return this;
	}

	MetalPipelineBuilder* MetalPipelineBuilder::SetMultisamplingCount(uint16_t multisamplingCount) {
		m_multisamplingCount = multisamplingCount;
		return this;
	}

	MetalPipelineBuilder*
	MetalPipelineBuilder::BindDescriptorLayouts(std::vector<std::shared_ptr<JarDescriptorLayout>> descriptors) {
		return this;
	}

	MetalPipelineBuilder*
	MetalPipelineBuilder::SetColorBlendAttachments(Graphics::ColorBlendAttachment blendAttachment) {
		auto colorAttachment = MTL::RenderPipelineColorAttachmentDescriptor::alloc()->init();
		colorAttachment->setPixelFormat(pixelFormatMap[blendAttachment.pixelFormat]);
		colorAttachment->setBlendingEnabled(blendAttachment.blendingEnabled);
		colorAttachment->setSourceRGBBlendFactor(blendFactorMap[blendAttachment.sourceRgbBlendFactor]);
		colorAttachment->setDestinationRGBBlendFactor(blendFactorMap[blendAttachment.destinationRgbBlendFactor]);
		colorAttachment->setRgbBlendOperation(blendOperationMap[blendAttachment.rgbBlendOperation]);
		colorAttachment->setSourceAlphaBlendFactor(blendFactorMap[blendAttachment.sourceAlphaBlendFactor]);
		colorAttachment->setDestinationAlphaBlendFactor(
				blendFactorMap[blendAttachment.destinationAlphaBlendFactor]);
		colorAttachment->setAlphaBlendOperation(blendOperationMap[blendAttachment.alphaBlendOperation]);
		colorAttachment->setWriteMask(convertToMetalColorWriteMask(blendAttachment.colorWriteMask));

		m_colorAttachments.push_back(colorAttachment);
		return this;
	}

	MetalPipelineBuilder*
	MetalPipelineBuilder::SetDepthStencilState(Graphics::DepthStencilState depthStencilState) {
		m_depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();

		if (depthStencilState.depthTestEnable) {
			m_depthStencilDescriptor->setDepthCompareFunction(compareFuncMap[depthStencilState.depthCompareOp]);
			m_depthStencilDescriptor->setDepthWriteEnabled(depthStencilState.depthWriteEnable);
		} else {
			m_depthStencilDescriptor->setDepthCompareFunction(MTL::CompareFunctionAlways);
		}

		if (depthStencilState.stencilTestEnable) {
			m_stencilDescriptor = MTL::StencilDescriptor::alloc()->init();
			m_stencilDescriptor->setStencilCompareFunction(compareFuncMap[depthStencilState.stencilCompareOp]);
			m_stencilDescriptor->setStencilFailureOperation(stencilOpMap[depthStencilState.stencilFailOp]);
			m_stencilDescriptor->setDepthFailureOperation(stencilOpMap[depthStencilState.stencilDepthFailOp]);
			m_stencilDescriptor->setDepthStencilPassOperation(stencilOpMap[depthStencilState.stencilPassOp]);
			m_stencilDescriptor->setReadMask(0xFF);
			m_stencilDescriptor->setWriteMask(0xFF);

			m_depthStencilDescriptor->setFrontFaceStencil(m_stencilDescriptor);
			m_depthStencilDescriptor->setBackFaceStencil(m_stencilDescriptor);
		}
		return this;
	}

	std::shared_ptr<JarPipeline> MetalPipelineBuilder::Build(std::shared_ptr<JarDevice> device) {
		auto metalDevice = reinterpret_cast<std::shared_ptr<MetalDevice>&>(device);
		MTL::RenderPipelineDescriptor* metalPipelineDesc = MTL::RenderPipelineDescriptor::alloc()->init();

		if (!m_vertexShaderFunc || !m_fragmentShaderFunc)
			throw std::runtime_error("Vertex and shader function must be set to build a valid pipeline!");

		metalPipelineDesc->setVertexFunction(m_vertexShaderFunc);
		metalPipelineDesc->setFragmentFunction(m_fragmentShaderFunc);

		if (m_vertexDescriptor)
			metalPipelineDesc->setVertexDescriptor(m_vertexDescriptor);

		metalPipelineDesc->setInputPrimitiveTopology(m_topology);
		metalPipelineDesc->setSampleCount(m_multisamplingCount);

		for (int i = 0; i < m_colorAttachments.size(); ++i) {
			MTL::RenderPipelineColorAttachmentDescriptor* colorAttachment = m_colorAttachments[i];
			metalPipelineDesc->colorAttachments()->setObject(colorAttachment, i);
		}
		auto mtlDevice = metalDevice->getDevice().value();

		NS::Error* error = nullptr;
		MTL::RenderPipelineState* pipelineState = mtlDevice->newRenderPipelineState(metalPipelineDesc, &error);
		if (!pipelineState) {
			throw std::runtime_error("Failed to create render pipeline state object! " +
			                         std::string(error->localizedDescription()->utf8String()));
		}

		std::optional<MTL::DepthStencilState*> depthStencilState = std::nullopt;
		if (m_depthStencilDescriptor) {
			depthStencilState = std::make_optional(metalDevice->getDevice().value()->
					newDepthStencilState(m_depthStencilDescriptor));
		}

		std::shared_ptr<MetalPipeline> metalPipeline = std::make_shared<MetalPipeline>(mtlDevice, pipelineState,
		                                                                               depthStencilState,
		                                                                               m_renderPass);
		return metalPipeline;
	}

	MTL::ColorWriteMask MetalPipelineBuilder::convertToMetalColorWriteMask(Graphics::ColorWriteMask mask) {
		MTL::ColorWriteMask metalMask = MTL::ColorWriteMaskNone;
		auto maskValue = static_cast<std::underlying_type<ColorWriteMask>::type>(mask);

		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Red))
			metalMask |= MTL::ColorWriteMaskRed;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Green))
			metalMask |= MTL::ColorWriteMaskGreen;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Blue))
			metalMask |= MTL::ColorWriteMaskBlue;
		if (maskValue & static_cast<std::underlying_type<ColorWriteMask>::type>(ColorWriteMask::Alpha))
			metalMask |= MTL::ColorWriteMaskAlpha;

		return metalMask;
	}
}