//
// Created by sebastian on 1/18/2025.
//
#pragma once

#include "core/VulkanCore.hpp"
#include "pipeline/VulkanFramebuffer.hpp"
#include "pipeline/VulkanPipeline.hpp"
#include "pipeline/VulkanRenderPass.hpp"
#include "pipeline/VulkanSwapchain.hpp"
#include "Rendering/IRenderer.hpp"

namespace JAREP::Rendering {
	struct RenderSettings;

	class RenderManager : public IRenderer {
		public:
			RenderManager();

			~RenderManager() override;

			bool Initialize(RenderSettings render_settings) override;

			void Resize(uint32_t width, uint32_t height) override;

			void DrawFrame() override;

			void Shutdown() override;

		private:
			std::unique_ptr<Core::VulkanCore> m_core;
			std::unique_ptr<Pipeline::VulkanSwapchain> m_swapchain;

			std::unique_ptr<Pipeline::VulkanRenderPass> m_renderPass;
			std::vector<std::unique_ptr<Pipeline::VulkanFramebuffer>> m_framebuffers;
			std::unique_ptr<Pipeline::VulkanPipeline> m_pipeline;
			VkPipelineLayout m_pipelineLayout;

			VkCommandPool m_commandPool;
			std::vector<VkCommandBuffer> m_commandBuffers;

			std::vector<VkSemaphore> m_imageAvailableSemaphores;
			std::vector<VkSemaphore> m_renderFinishedSemaphores;
			std::vector<VkFence> m_inFlightFences;
			size_t m_currentFrame;
			uint32_t m_acquiredImageIndex;

			void initSwapchain(uint32_t width, uint32_t height);

			void createRenderPass();

			void createFramebuffers();

			void createPipeline();

			void beginFrame();

			void recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imageIndex);

			void endFrame(uint32_t imageIndex);
	};
}
