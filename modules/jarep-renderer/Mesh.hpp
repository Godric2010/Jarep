//
// Created by sebastian on 11.02.24.
//

#ifndef JAREP_MESH_HPP
#define JAREP_MESH_HPP

#include <utility>
#include <vector>
#include "Vertex.hpp"

class Mesh {
	public:
		Mesh(std::vector<Graphics::Vertex> verts, std::vector<uint16_t> idxs) : vertices(std::move(verts)),
		                                                                        indices(std::move(idxs)) {}

		~Mesh() = default;

		[[nodiscard]]const std::vector<Graphics::Vertex>& getVertices() const { return vertices; }

		[[nodiscard]]const std::vector<uint16_t>& getIndices() const { return indices; }

	private:
		std::vector<Graphics::Vertex> vertices;
		std::vector<uint16_t> indices;
};

#endif //JAREP_MESH_HPP
