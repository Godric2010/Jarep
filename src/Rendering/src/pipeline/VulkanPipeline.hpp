//
// Created by Godri on 3/29/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>
#include <optional>

namespace JAREP::Rendering::Pipeline {
	/**
	 * @brief Configuration for Vulkan graphics pipeline
	 */
	struct VulkanPipelineConfig {
		VkDevice device;
		VkRenderPass renderPass;
		VkExtent2D extent;

		std::string vertexShaderPath;
		std::string fragmentShaderPath;

		VkPipelineLayout pipelineLayout;

		VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
		VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

		bool depthTestEnable = true;
		bool depthWriteEnable = true;
		std::optional<VkFormat> depthFormat = std::nullopt;

	};

	/**
	 * @class VulkanPipeline
	 * @brief Encapsulates the creation of a Vulkan graphics pipeline
	 */
	class VulkanPipeline {
		public:
		explicit VulkanPipeline(const VulkanPipelineConfig& config);
		~VulkanPipeline();
		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		VkPipeline get() const;

		private:
		void createShaderModule(const std::string& filePath, VkShaderModule* shaderModule);
		void createGraphicsPipeline();

		VkDevice m_device;
		VulkanPipelineConfig m_config;

		VkPipeline m_pipeline;
		VkShaderModule m_vertexShaderModule;
		VkShaderModule m_fragmentShaderModule;
	};
}
