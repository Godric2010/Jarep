//
// Created by Godri on 4/13/2025.
//

#pragma once
#include <unordered_map>

#include "EngineCore/IMeshLibrary.hpp"
#include "Hashing/TypeHasher.hpp"

namespace JAREP::Core {
	class MeshLibrary : public IMeshLibrary {
		public:
			MeshLibrary();

			~MeshLibrary() override;

			MeshID AddMesh(const Types::Mesh&mesh) override;

			void RemoveMesh(MeshID id) override;

			[[nodiscard]] std::shared_ptr<Types::Mesh> GetMesh(MeshID id) const override;

		private:
			std::unordered_map<MeshID, std::shared_ptr<Types::Mesh>> m_meshes;
	};
}
