//
// Created by Godri on 3/30/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include "IRenderStep.hpp"
#include "../../include/Rendering/Core/RenderObject.hpp"
#include "Rendering/Core/CameraObject.hpp"

namespace JAREP::Rendering::Steps {
	class RenderStepManager {
		public:
			RenderStepManager();

			~RenderStepManager();

			void AddStep(std::unique_ptr<IRenderStep> step);

			void Execute(VkCommandBuffer commandBuffer) const;

			void SetRenderResolution(VkExtent2D resolution) const;

			void SetRenderObjects(std::vector<Core::RenderObject> renderObjects) const;

			void Resize(VkExtent2D newExtent) const;

		private:
			std::vector<std::unique_ptr<IRenderStep>> m_renderSteps;
	};
}
