//
// Created by Godri on 5/7/2025.
//

#pragma once
#include <array>

namespace JAREP::Rendering::Core {
	struct alignas(16) CameraUBO {
		std::array<float, 16> viewMatrix;
		std::array<float, 16> projectionMatrix;
		uint32_t isOrthographic;
		uint32_t padding[3];
	};

	struct CameraConfig {
		CameraUBO cameraUBO;
		float nearPlane;
		float farPlane;
		bool cullBackFaces;
		bool cullFrontFaces;
	};
}
