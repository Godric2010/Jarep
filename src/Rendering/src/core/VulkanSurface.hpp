//
// Created by sebastian on 3/22/2025.
//

#pragma once
#include <vulkan/vulkan.h>
#include "../include/Rendering/IRenderer.hpp"

struct SDL_Window;

namespace JAREP::Rendering::Core {
	class VulkanSurface {
		/**
		 *Represents the platform-specific rendering surface. Connects the Vulkan instance to the windowing system (here SDL2)
		 **/
		public:
			VulkanSurface(VkInstance instance, RenderSettings render_settings);

			~VulkanSurface();

			VulkanSurface(const VulkanSurface&) = delete;

			VulkanSurface& operator=(const VulkanSurface&) = delete;

			VkSurfaceKHR get() const;

		private:
			VkInstance m_instance;
			VkSurfaceKHR m_surface;

			void createSurface(void* handle, void* display);
	};
}
