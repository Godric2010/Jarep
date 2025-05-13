//
// Created by Godri on 4/22/2025.
//

#pragma once
#include "EngineCore/IMeshLibrary.hpp"

namespace JAREP::Rendering::Core {
	struct alignas(16) InstanceData {
		std::array<float, 16> modelMatrix;
	};

	struct RenderObject {
		JAREP::Core::MeshID meshID;
		InstanceData transform;
	};
}
