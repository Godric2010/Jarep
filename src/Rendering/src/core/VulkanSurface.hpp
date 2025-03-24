//
// Created by sebastian on 3/22/2025.
//

#pragma once
#include <vulkan/vulkan.h>
#include <windows.h>
#include<vulkan/vulkan_win32.h>

#include "Rendering/IRenderer.hpp"

struct SDL_Window;
namespace JAREP::Rendering::Core{
class VulkanSurface {
    public:
      VulkanSurface(VkInstance instance, RenderSettings render_settings);
      ~VulkanSurface();

      VkSurfaceKHR get() const;

      private:
        VkInstance m_instance;
        VkSurfaceKHR m_surface;

		void createSurfaceWin(void* handle, void* display);
		void createSurfaceX11(void* handle, void* display);
};

}

