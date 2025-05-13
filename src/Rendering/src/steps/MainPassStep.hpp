//
// Created by Godri on 3/30/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>

#include "IRenderData.hpp"
#include "IRenderStep.hpp"
#include "../core/VulkanDescriptorPool.hpp"
#include "../core/VulkanDescriptorSet.hpp"
#include "../core/VulkanDescriptorSetLayout.hpp"
#include "../core/VulkanUniformBuffer.hpp"

#include "../meshes/VulkanMeshRegistry.hpp"
#include "../pipeline/VulkanOffscreenTarget.hpp"
#include "../pipeline/VulkanPipeline.hpp"
#include "../pipeline/VulkanRenderPass.hpp"
#include "Rendering/Core/CameraObject.hpp"

namespace JAREP::Rendering::Pipeline {
	class VulkanDepthBuffer;
}

namespace JAREP::Rendering::Steps {
	class MainPassStep : public IRenderStep, public IRenderData {
		public:
			MainPassStep(VkDevice device, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits sampleCountFlags,
			             Meshes::VulkanMeshRegistry* meshRegistry, const CameraConfig&cameraConfig);

			~MainPassStep() override;

			MainPassStep(const MainPassStep&) = delete;

			MainPassStep& operator=(const MainPassStep&) = delete;

			void Prepare(VkExtent2D extent, VkFormat format) override;

			void BindToOutputOf(IRenderStep* previousRenderStep) override;

			void Resize(VkExtent2D newExtent) override;

			void Record(VkCommandBuffer cmdBuffer) override;

			VkImageView GetOutputImageView() override;

			VkImage GetOutputImage() override;

			void SetRenderTargetObjects(std::vector<Core::RenderObject>&renderObjects) override;

		private:
			void createFramebuffer();

			void createDepthImage();

			void createRenderPass();

			void createPipeline();


			VkDevice m_device;
			VkPhysicalDevice m_physicalDevice;
			VkExtent2D m_extent;
			VkFormat m_format;
			VkPipelineLayout m_pipelineLayout;
			VkSampleCountFlagBits m_sampleCountFlag;

			std::unique_ptr<Pipeline::VulkanDepthBuffer> m_depthImageBuffer;
			std::unique_ptr<Pipeline::VulkanOffscreenTarget> m_offscreenTarget;
			std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;
			std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
			std::unique_ptr<VulkanDescriptorPool> m_descriptorPool;
			std::unique_ptr<VulkanDescriptorSet> m_descriptorSet;
			std::unique_ptr<VulkanDescriptorSetLayout> m_descriptorSetLayout;

			Meshes::VulkanMeshRegistry* m_meshRegistry;
			CameraConfig m_cameraConfig;
			std::vector<RenderObject> m_renderObjects;
			std::unique_ptr<VulkanUniformBuffer<CameraUBO>> m_cameraUBO;
			std::unique_ptr<VulkanUniformBuffer<InstanceData>> m_objectUBO;
	};
}
