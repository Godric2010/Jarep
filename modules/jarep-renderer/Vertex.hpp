//
// Created by sebastian on 12.11.23.
//

#ifndef JAREP_VERTEX_HPP
#define JAREP_VERTEX_HPP

/*
#include <glm/glm.hpp>
#include "API/src/IRendererAPI/IRenderAPI.hpp"

namespace Graphics {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoord;


		static std::vector<AttributeDescription> GetAttributeDescriptions() {
			std::vector attributeDescriptions = {AttributeDescription{}, AttributeDescription{},
			                                     AttributeDescription{}};
			attributeDescriptions[0].vertexFormat = VertexFormat::Float3;
			attributeDescriptions[0].offset = offsetof(Vertex, position);
			attributeDescriptions[0].bindingIndex = 0;
			attributeDescriptions[0].attributeLocation = 0;

			attributeDescriptions[1].vertexFormat = VertexFormat::Float3;
			attributeDescriptions[1].offset = offsetof(Vertex, color);
			attributeDescriptions[1].bindingIndex = 0;
			attributeDescriptions[1].attributeLocation = 1;

			attributeDescriptions[2].bindingIndex = 0;
			attributeDescriptions[2].attributeLocation = 2;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
			attributeDescriptions[2].vertexFormat = VertexFormat::Float2;

			return attributeDescriptions;
		}

		static std::vector<BindingDescription> GetBindingDescriptions() {
			std::vector bindingDescriptions = {BindingDescription{}};
			bindingDescriptions[0].bindingIndex = 0;
			bindingDescriptions[0].inputRate = VertexInputRate::PerVertex;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].stepRate = 1;
			return bindingDescriptions;
		}
	};
}

#endif //JAREP_VERTEX_HPP
*/