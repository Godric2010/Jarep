//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANGRAPHICSPIPELINEBUILDER_HPP
#define JAREP_VULKANGRAPHICSPIPELINEBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanDataTypeMaps.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>
#include "VulkanBackend.hpp"

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanBuffer;

	class VulkanRenderPass;

	class VulkanGraphicsPipelineBuilder final : public JarPipelineBuilder {
		public:
			VulkanGraphicsPipelineBuilder();

			~VulkanGraphicsPipelineBuilder() override;

			VulkanGraphicsPipelineBuilder* SetShaderStage(ShaderStage shaderStage) override;

			VulkanGraphicsPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			VulkanGraphicsPipelineBuilder* SetVertexInput(VertexInput vertexInput) override;

			VulkanGraphicsPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) override;

			VulkanGraphicsPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) override;

			VulkanGraphicsPipelineBuilder*
			BindDescriptorLayouts(std::vector<std::shared_ptr<JarDescriptorLayout>> descriptorLayouts) override;

			VulkanGraphicsPipelineBuilder* SetColorBlendAttachments(ColorBlendAttachment colorBlendAttachment) override;

			VulkanGraphicsPipelineBuilder* SetDepthStencilState(DepthStencilState depthStencilState) override;

			std::shared_ptr<JarPipeline> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
			std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
			std::optional<VkPipelineVertexInputStateCreateInfo> m_vertexInput;
			std::optional<VkPipelineInputAssemblyStateCreateInfo> m_inputAssembly;
			std::optional<VkPipelineMultisampleStateCreateInfo> m_multisampling;
			std::vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachmentStates;
			std::optional<VkPipelineColorBlendStateCreateInfo> m_colorBlend;
			std::optional<VkPipelineDepthStencilStateCreateInfo> m_depthStencil;
			std::optional<VkPipelineRasterizationStateCreateInfo> m_rasterization;
			std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
			std::vector<std::shared_ptr<VulkanBuffer>> m_uniformBuffers;
			std::shared_ptr<VulkanRenderPass> m_renderPass;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_pipeline;

			static VkColorComponentFlags convertToColorComponentFlagBits(ColorWriteMask colorWriteMask);

	};
}

#endif //JAREP_VULKANGRAPHICSPIPELINEBUILDER_HPP
