//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANCOMMANDBUFFER_HPP
#define JAREP_VULKANCOMMANDBUFFER_HPP

#include "IRenderAPI.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandQueue.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanDevice;

	class VulkanCommandQueue;

	class VulkanFramebuffer;

	class VulkanGraphicsPipeline;

	class VulkanCommandBuffer final : JarCommandBuffer {
		public:
		VulkanCommandBuffer(VkCommandBuffer commandBuffer, std::shared_ptr<VulkanDevice> device);

		~VulkanCommandBuffer() override;

		void SetDepthBias(DepthBias depthBias) override;

		void SetViewport(Viewport viewport) override;

		void SetScissor(Scissor scissor) override;

		bool
		StartRecording(std::shared_ptr<JarFramebuffer> framebuffer, std::shared_ptr<JarRenderPass> renderPass) override;

		void EndRecording() override;

		void BindPipeline(std::shared_ptr<JarPipeline> pipeline, uint32_t frameIndex) override;

		void BindDescriptors(std::vector<std::shared_ptr<JarDescriptor>> descriptors) override;

		void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) override;

		void BindIndexBuffer(std::shared_ptr<JarBuffer> indexBuffer) override;

		void Draw() override;

		void DrawIndexed(size_t indexAmount) override;

		void BlitFramebuffersToSurface(std::shared_ptr<JarSurface> surface, std::vector<std::shared_ptr<JarFramebuffer>> framebuffers) override;

		void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

		void Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) override;

		void Release(VkDevice device);

		static VkCommandBuffer StartSingleTimeRecording(std::shared_ptr<VulkanDevice>& device,
		                                                std::shared_ptr<VulkanCommandQueue> commandQueue);

		static void EndSingleTimeRecording(
		        std::shared_ptr<VulkanDevice>& device,
		        VkCommandBuffer commandBuffer,
		        std::shared_ptr<VulkanCommandQueue>& commandQueue);

		std::optional<std::shared_ptr<VulkanGraphicsPipeline>> m_pipeline;

		private:
		std::shared_ptr<VulkanDevice> m_device;
		VkCommandBuffer m_commandBuffer;
		VkSemaphore m_imageAvailableSemaphore;
		VkSemaphore m_renderFinishedSemaphore;
		VkSemaphore m_renderStepSemaphore;
		VkSemaphore m_blitSemaphore;
		VkFence m_frameInFlightFence;

		void CreateSemaphore(VkSemaphore& semaphore);
		void CreateFence(VkFence& fence);
	};
}// namespace Graphics::Vulkan

#endif//JAREP_VULKANCOMMANDBUFFER_HPP
