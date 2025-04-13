//
// Created by Godri on 4/13/2025.
//

#pragma once
#include <glm/glm.hpp>

namespace JAREP::Core::Types {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};
}
