//
// Created by Godri on 4/22/2025.
//

#pragma once
#include "Rendering/RenderObject.hpp"

namespace JAREP::Rendering::Steps {
	class IRenderData {
		public:
			virtual ~IRenderData() = default;
			virtual void SetRenderTargetObjects(std::vector<Core::RenderObject>&renderObjects) = 0;
	};
}
