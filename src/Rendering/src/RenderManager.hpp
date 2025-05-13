//
// Created by sebastian on 1/18/2025.
//
#pragma once

#include "core/VulkanCore.hpp"
#include "meshes/VulkanMeshRegistry.hpp"
#include "pipeline/VulkanFramebuffer.hpp"
#include "pipeline/VulkanPipeline.hpp"
#include "pipeline/VulkanRenderPass.hpp"
#include "pipeline/VulkanSwapchain.hpp"
#include "Rendering/IRenderer.hpp"
#include "steps/RenderStepManager.hpp"

namespace JAREP::Rendering {
	struct RenderSettings;
	constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	class RenderManager : public IRenderer {
		public:
			RenderManager();

			~RenderManager() override;

			bool Initialize(RenderSettings renderSettings, Core::CameraConfig cameraConfig) override;

			void Resize(uint32_t width, uint32_t height) override;

			void SetRenderResolution(uint32_t resX, uint32_t resY) override;

			void AddRenderObject(JAREP::Core::MeshID meshID, std::shared_ptr<JAREP::Core::Types::Mesh> mesh,
			                     Core::InstanceData objectData) override;

			void UpdateRenderObject(Core::RenderObject renderObject) override;

			void DrawFrame() override;

			void Shutdown() override;

		private:
			std::unique_ptr<Core::VulkanCore> m_core;
			std::unique_ptr<Pipeline::VulkanSwapchain> m_swapchain;

			std::unique_ptr<Steps::RenderStepManager> m_renderStepManager;

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
			VkExtent2D m_windowResolution;
			VkExtent2D m_renderResolution;
			VkSampleCountFlagBits m_sampleCountFlag;

			std::unique_ptr<Meshes::VulkanMeshRegistry> m_meshRegistry;

			void clampAndSetSampleCount(uint8_t requestedSample);

			void initSwapchain(uint32_t width, uint32_t height);

			void createRenderSteps(Core::CameraConfig cameraConfig) const;

			void createSyncObjects();

			void allocateCommandBuffers();

			void destroySyncObjects();

			void beginFrame();

			void endFrame(uint32_t imageIndex);
	};
}
