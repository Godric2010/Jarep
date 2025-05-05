//
// Created by Godri on 4/13/2025.
//
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "../src/MeshLibrary.hpp"
#include "EngineCore/IMeshLibrary.hpp"
#include "EngineCore/Types/Mesh.hpp"
#include "EngineCore/Types/Vertex.hpp"

using namespace JAREP::Core;

TEST_CASE("IMeshLibrary basic functionality tests", "[MeshLibrary]") {
	IMeshLibrary* meshLibrary = new MeshLibrary();

	Types::Mesh meshA = {};
	meshA.vertices = {{{1.0, 0.0, 0.0}, {0.0, 0.0}}};
	meshA.indices = {0};

	Types::Mesh meshB = {};
	meshB.vertices = {{{2.0, 1.0, 0.0}, {0.5, 0.5}}};
	meshB.indices = {0};

	SECTION("Mesh is loaded and returns valid ID") {
		MeshID idA = meshLibrary->AddMesh(meshA);
		REQUIRE(idA != 0);
	}

	SECTION("Duplicate returns same ID") {
		MeshID id1 = meshLibrary->AddMesh(meshA);
		MeshID id2 = meshLibrary->AddMesh(meshA);
		REQUIRE(id1 == id2);
	}

	SECTION("Different meshes yield different IDs") {
		MeshID id1 = meshLibrary->AddMesh(meshA);
		MeshID id2 = meshLibrary->AddMesh(meshB);
		REQUIRE(id1 != id2);
	}
	SECTION("Meshes can be retrieved correctly") {
		MeshID idA = meshLibrary->AddMesh(meshA);
		// std::shared_ptr<Types::Mesh> retrieved = meshLibrary->GetMesh(idA);
		// REQUIRE(retrieved.vertices == meshA.vertices);
		// REQUIRE(retrieved.indices == meshA.indices);
	}

	SECTION("Getting unknown mesh throws exception") {
		REQUIRE_THROWS_AS(meshLibrary->GetMesh(9999), std::out_of_range);
	}

	SECTION("Unloading mesh removes it") {
		MeshID idA = meshLibrary->AddMesh(meshA);
		meshLibrary->RemoveMesh(idA);
		REQUIRE_THROWS_AS(meshLibrary->GetMesh(idA), std::out_of_range);
	}

	SECTION("Unloading unknown mesh throws exception") {
		REQUIRE_THROWS_AS(meshLibrary->RemoveMesh(9999), std::out_of_range);
	}
}
