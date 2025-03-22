//
// Created by sebastian on 3/22/2025.
//

#include "VulkanSurface.hpp"
#include <SDL_vulkan.h>
#include <stdexcept>

using namespace JAREP::Rendering::Core;

VulkanSurface::VulkanSurface(VkInstance instance, RenderSettings render_settings) {
    m_instance = instance;
    m_surface = VK_NULL_HANDLE;

    if(!SDL_Vulkan_CreateSurface(instance, window, &m_surface)) {
      throw std::runtime_error("Failed to create Vulkan surface!");
    }
}

VulkanSurface::~VulkanSurface() {
  if(m_surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  }
}

VkSurfaceKHR VulkanSurface::get() const {
  return m_surface;
}