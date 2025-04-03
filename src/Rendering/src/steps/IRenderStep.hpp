//
// Created by Godri on 3/30/2025.
//

#pragma once
#include <vulkan/vulkan.hpp>

namespace JAREP::Rendering::Steps {
	class IRenderStep {
		public:
			virtual ~IRenderStep() = default;

			/**
			 * Prepare the render step with e.g. Resize, Resource-Rebuild, etc.
			 * @param extent The extent of the framebuffer to render to.
			 * @param format The image format used for this render step.
			 */
			virtual void Prepare(VkExtent2D extent, VkFormat format) = 0;

			/**
			 * Bind the output of another render step to this one, to create a chain of processing.
			 * @param previousRenderStep The previous render step which output shall be used.
			 */
			virtual void BindToOutputOf(IRenderStep* previousRenderStep) = 0;

			/**
			 * Resize all render steps to a new render resolution.
			 * @param newExtent The new render resolution that shall be applied to the steps framebuffer(s).
			 */
			virtual void Resize(VkExtent2D newExtent) = 0;

			/**
			 * Record the command buffer, draw and dispatch.
			 * @param cmd The command buffer to record into.
			 */
			virtual void Record(VkCommandBuffer cmd) = 0;

			/**
			 * Return the result of this render step as VKImageView.
			 * @return The result VkImageView of this Render Step
			 */
			virtual VkImageView GetOutput() = 0;
	};
}
