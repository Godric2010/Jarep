//
// Created by Godri on 3/30/2025.
//

#include "RenderStepManager.hpp"

#include "IRenderData.hpp"

using namespace JAREP::Rendering::Steps;

RenderStepManager::RenderStepManager() {
	m_renderSteps = std::vector<std::unique_ptr<IRenderStep>>();
}

RenderStepManager::~RenderStepManager() {
	m_renderSteps.clear();
}

void RenderStepManager::AddStep(std::unique_ptr<IRenderStep> step) {
	m_renderSteps.push_back(std::move(step));
}

void RenderStepManager::Execute(const VkCommandBuffer commandBuffer) const {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	for (const auto&step: m_renderSteps) {
		step->Record(commandBuffer);
	}

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void RenderStepManager::SetRenderResolution(const VkExtent2D resolution) const {
	for (size_t i = 0; i < m_renderSteps.size() - 1; i++) {
		m_renderSteps[i]->Resize(resolution);
	}
}

void RenderStepManager::SetRenderObjects(std::vector<Core::RenderObject> renderObjects) const {
	for (const auto&step: m_renderSteps) {
		if (auto* dataReceiver = dynamic_cast<IRenderData *>(step.get())) {
			dataReceiver->SetRenderTargetObjects(renderObjects);
		}
	}
}

void RenderStepManager::Resize(const VkExtent2D newExtent) const {
	size_t lastStep = m_renderSteps.size() - 1;
	m_renderSteps[lastStep]->Resize(newExtent);
}
