//
// Created by Godri on 4/13/2025.
//

#pragma once
#include "Types/Mesh.hpp"

namespace JAREP::Core {
	struct MeshID {
		size_t value;
	};

	class IMeshLibrary {
		public:
			virtual ~IMeshLibrary() = 0;

			virtual size_t AddMesh(Types::Mesh mesh) = 0;

			virtual Types::Mesh GetMesh(size_t index) = 0;
	};
}
