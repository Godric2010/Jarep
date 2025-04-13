//
// Created by Godri on 4/13/2025.
//

#pragma once
#include "Types/Mesh.hpp"

namespace JAREP::Core {
	using MeshID = size_t;

	/**
	 * The mesh library contains all meshes available into the memory at runtime.
	 */
	class IMeshLibrary {
		public:
			virtual ~IMeshLibrary() = default;

			/**
			 * Add a new mesh to the library in memory.
			 * @param mesh The mesh to add.
			 * @return The ID of the Mesh inside the library (HashCode)
			 */
			virtual MeshID LoadMesh(Types::Mesh mesh) = 0;

			/**
			 * Remove a mesh from the library and therefore the memory.
			 * @param id The id of the mesh to remove.
			 */
			virtual void UnloadMesh(MeshID id) = 0;

			/**
			 * Get a mesh from the library.
			 * @param id The hash-id of the mesh.
			 * @return The mesh behind this id.
			 */
			[[nodiscard]] virtual Types::Mesh GetMesh(MeshID id) const = 0;
	};
}
