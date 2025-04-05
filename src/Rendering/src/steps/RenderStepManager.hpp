//
// Created by Godri on 3/30/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include "IRenderStep.hpp"

namespace JAREP::Rendering::Steps {
	class RenderStepManager {
		public:
			RenderStepManager();

			~RenderStepManager();

			void AddStep(std::unique_ptr<IRenderStep> step);

			void Execute(VkCommandBuffer commandBuffer);

			void SetRenderResolution(VkExtent2D resolution);

			void Resize(VkExtent2D newExtent);

		private:
			std::vector<std::unique_ptr<IRenderStep>> m_renderSteps;
	};
}
