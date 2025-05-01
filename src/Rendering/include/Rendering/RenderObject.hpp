//
// Created by Godri on 4/22/2025.
//

#pragma once
#include "EngineCore/IMeshLibrary.hpp"

namespace JAREP::Rendering::Core {
	struct alignas(16) ObjectUBO {
		std::array<float, 16> transformMatrix;
	};

	struct RenderObject {
		JAREP::Core::MeshID meshID;
		ObjectUBO transform;
	};
}
