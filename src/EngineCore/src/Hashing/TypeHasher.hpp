//
// Created by Godri on 4/13/2025.
//

#pragma once

#include "EngineCore/Types/Mesh.hpp"
#include "EngineCore/Types/Vertex.hpp"

namespace JAREP::Core::Hashing {
	inline size_t HashCombine(const size_t lhs, const size_t rhs) {
		return lhs ^ (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
	}

	inline size_t HashVertex(const Types::Vertex&vertex) {
		size_t h = std::hash<float>()(vertex.position.x);
		h = HashCombine(h, std::hash<float>()(vertex.position.y));
		h = HashCombine(h, std::hash<float>()(vertex.position.z));
		h = HashCombine(h, std::hash<float>()(vertex.uv.x));
		h = HashCombine(h, std::hash<float>()(vertex.uv.y));
		return h;
	}

	inline size_t HashMesh(const Types::Mesh&mesh) {
		size_t h = 0;
		for (const auto&vertex: mesh.vertices) {
			h = HashCombine(h, HashVertex(vertex));
		}
		for (const auto&index: mesh.indices) {
			h = HashCombine(h, std::hash<uint32_t>()(index));
		}
		return h;
	}
}
