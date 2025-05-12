//
// Created by Godri on 5/7/2025.
//

#pragma once
#include "Types/Transform.hpp"

namespace JAREP::Core {
	struct Camera {
		Types::Transform transform;
		float fovY;
		float nearZ;
		float farZ;
		bool isOrthographic;
		float orthographicWidth;
		float orthographicHeight;
		bool cullBackFaces;
		bool cullFrontFaces;
	};
}
