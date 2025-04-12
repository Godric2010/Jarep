//
// Created by Godri on 3/30/2025.
//

#pragma once
#include <functional>
#include <vulkan/vulkan.hpp>

#include "IRenderStep.hpp"
#include "../pipeline/VulkanOffscreenTarget.hpp"
#include "../pipeline/VulkanPipeline.hpp"
#include "../pipeline/VulkanRenderPass.hpp"

namespace JAREP::Rendering::Pipeline {
	class VulkanDepthBuffer;
}

namespace JAREP::Rendering::Steps {
	class MainPassStep : public IRenderStep {
		public:
			MainPassStep(VkDevice device, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits sampleCountFlags);

			~MainPassStep() override;

			MainPassStep(const MainPassStep&) = delete;

			MainPassStep& operator=(const MainPassStep&) = delete;

			void Prepare(VkExtent2D extent, VkFormat format) override;

			void BindToOutputOf(IRenderStep* previousRenderStep) override;

			void Resize(VkExtent2D newExtent) override;

			void Record(VkCommandBuffer cmdBuffer) override;

			VkImageView GetOutputImageView() override;

			VkImage GetOutputImage() override;

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
	};
}
