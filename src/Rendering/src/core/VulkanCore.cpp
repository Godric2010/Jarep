//
// Created by sebastian on 1/18/2025.
//

#include "VulkanCore.hpp"
using namespace JAREP::Rendering::Core;

VulkanCore::VulkanCore() = default;

VulkanCore::~VulkanCore() = default;

bool VulkanCore::Initialize(std::vector<const char*> extensions) {
     m_instance = std::make_unique<VulkanInstance>(extensions);
     return true;
}

void VulkanCore::Shutdown() {

     m_instance->Destroy();
}
