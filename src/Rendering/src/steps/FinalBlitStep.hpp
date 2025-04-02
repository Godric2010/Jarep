//
// Created by Godri on 3/30/2025.
//

#pragma once
#include "IRenderStep.hpp"

namespace JAREP::Rendering::Pipeline {
	class VulkanPipeline;
	class VulkanRenderPass;
}

namespace JAREP::Rendering::Steps {
	class FinalBlitStep : public IRenderStep {
		public:
		FinalBlitStep(VkDevice device, VkPhysicalDevice physicalDevice);
		~FinalBlitStep() override;
		FinalBlitStep(const FinalBlitStep&) = delete;
		FinalBlitStep& operator=(const FinalBlitStep&) = delete;

		void Prepare(VkExtent2D extent, VkFormat format) override;
		void Resize(VkExtent2D newExtent) override;
		void Record(VkCommandBuffer cmd) override;
		VkImageView GetOutput() override;

		private:

		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
		VkExtent2D m_extent;
		VkFormat m_format;
		VkPipelineLayout m_pipelineLayout;

		std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
		std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;

	};
}
