//
// Created by sebastian on 1/18/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanInstance.hpp"

namespace JAREP::Rendering::Core{

class VulkanCore {
    public:
      VulkanCore();
      ~VulkanCore();

      bool Initialize();
      void Shutdown();

    private:
        std::unique_ptr<VulkanInstance> m_instance;

};
}

