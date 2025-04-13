//
// Created by Godri on 4/13/2025.
//

#pragma once
#include <map>
#include "EngineCore/IMeshLibrary.hpp"

namespace JAREP::Core {
	class MeshLibrary : public IMeshLibrary {
		public:
			MeshLibrary();

			~MeshLibrary() override;

			size_t AddMesh(Types::Mesh mesh) override;

			Types::Mesh GetMesh(size_t index) override;

		private:
			std::map<size_t, Types::Mesh> m_meshes;
			size_t m_lastIndex;
	};
}
