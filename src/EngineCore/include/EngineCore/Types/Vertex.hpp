//
// Created by Godri on 4/13/2025.
//

#pragma once
#include <glm/glm.hpp>

namespace JAREP::Core::Types {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;

		bool operator==(const Vertex&other) const {
			return position == other.position && uv == other.uv;
		}

		bool operator!=(const Vertex&other) const {
			return !(*this == other);
		}
	};
}
