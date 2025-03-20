//
// Created by sebastian on 1/18/2025.
//

#include "RenderManager.hpp"

using namespace JAREP::Rendering;

RenderManager::RenderManager() = default;

RenderManager::~RenderManager() = default;

bool RenderManager::Initialize(std::vector<const char *> extensions) {
	// Initialize Core systems
	m_core = std::make_unique<Core::VulkanCore>();
	m_core->Initialize(extensions);

	// Initialize Pipelines and Render passes

	// Allocate resources

	return true;
}

void RenderManager::Shutdown() {
	// Free resources

	// Destroy render passes and pipelines

	// Destroy core elements
}
