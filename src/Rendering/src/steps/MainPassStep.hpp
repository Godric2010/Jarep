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

namespace JAREP::Rendering::Steps {
	class MainPassStep : public IRenderStep {
		public:
			MainPassStep(VkDevice device, VkPhysicalDevice physicalDevice);

			~MainPassStep() override;

			MainPassStep(const MainPassStep&) = delete;

			MainPassStep& operator=(const MainPassStep&) = delete;

			void Prepare(VkExtent2D extent, VkFormat format, VkQueue graphicsQueue, VkCommandPool commandPool) override;

			void BindToOutputOf(IRenderStep *previousRenderStep) override;

			void Resize(VkExtent2D newExtent) override;

			void Record(VkCommandBuffer cmdBuffer) override;

			VkImageView GetOutputImageView() override;

			VkImage GetOutputImage() override;

		private:

			void createFramebuffer(VkCommandPool commandPool, VkQueue queue);
			void createRenderPass();
			void createPipeline();


			VkDevice m_device;
			VkPhysicalDevice m_physicalDevice;
			VkExtent2D m_extent;
			VkFormat m_format;
			VkPipelineLayout m_pipelineLayout;
			VkCommandPool m_commandPool;
			VkQueue m_queue;

			std::unique_ptr<Pipeline::VulkanOffscreenTarget> m_offscreenTarget;
			std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;
			std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
	};
}
