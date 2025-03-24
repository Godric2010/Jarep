//
// Created by Godri on 3/24/2025.
//

#if defined(__linux__)

#include "VulkanSurface.hpp"
#include <vulkan/vulkan_xlib.h>
#include <X11/Xlib.h>

using namespace JAREP::Rendering::Core;

void VulkanSurface::createSurface(void* handle, void* display){
  VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.dpy = reinterpret_cast<Display*>(display);
  surfaceCreateInfor.window = reinterpret_cast<Window>(display);

  if(vkCreateXlibSurfaceKHR(display, &surfaceCreateInfo, nullptr, &surface)){
    throw std::runtime_error("failed to create Xlib surface");
  }
}
#endif
