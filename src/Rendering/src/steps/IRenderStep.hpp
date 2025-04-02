//
// Created by Godri on 3/30/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Steps {
	class IRenderStep {
		public:
			virtual ~IRenderStep() = default;

			/*
			* Prepare the render step with e.g. Resize, Resource-Rebuild, etc.
			*/
			virtual void Prepare(VkExtent2D extent, VkFormat format) = 0;

			/*
			 * Resize all render steps to a new render resolution.
			 */
			virtual void Resize(VkExtent2D newExtent) = 0;

			/*
			* Record the command buffer, draw and dispatch
			*/
			virtual void Record(VkCommandBuffer cmd) = 0;

			/*
			* Use for chaining render steps
			*/
			virtual VkImageView GetOutput() = 0;
	};
}
