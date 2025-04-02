//
// Created by Godri on 3/30/2025.
//

#include "RenderStepManager.hpp"

using namespace JAREP::Rendering::Steps;

RenderStepManager::RenderStepManager() {
	m_renderSteps = {};
}

RenderStepManager::~RenderStepManager() {

}

void RenderStepManager::AddStep(std::unique_ptr<IRenderStep> step) {
	m_renderSteps.push_back(std::move(step));
}

void RenderStepManager::Execute(VkCommandBuffer commandBuffer) {
	for (const auto& step : m_renderSteps) {
		step->Record(commandBuffer);
	}
}

void RenderStepManager::Resize(VkExtent2D newExtent) {
	for (const auto& step : m_renderSteps) {
		step->Prepare(TODO, TODO);
	}
}