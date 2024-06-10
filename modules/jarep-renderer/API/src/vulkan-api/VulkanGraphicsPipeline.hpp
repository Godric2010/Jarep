//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANGRAPHICSPIPELINE_HPP
#define JAREP_VULKANGRAPHICSPIPELINE_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanRenderPass;

	class VulkanGraphicsPipeline final : public JarPipeline {
		public:
			VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice>& device, VkPipelineLayout pipelineLayout,
			                       VkPipeline pipeline, std::shared_ptr<VulkanRenderPass>& renderPass) :
					m_device(device), m_pipelineLayout(pipelineLayout), m_graphicsPipeline(pipeline),
					m_renderPass(renderPass) {}

			~VulkanGraphicsPipeline() override;

			void Release() override;

			std::shared_ptr<JarRenderPass> GetRenderPass() override;

			[[nodiscard]] VkPipeline getPipeline() const { return m_graphicsPipeline; }

			[[nodiscard]] VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_graphicsPipeline;
			std::shared_ptr<VulkanRenderPass> m_renderPass;
	};
}

#endif //JAREP_VULKANGRAPHICSPIPELINE_HPP
