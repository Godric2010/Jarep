//
// Created by Godri on 4/13/2025.
//

#pragma once

#include <vector>
#include "Vertex.hpp"

namespace JAREP::Core::Types {
	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
}
