#include <iostream>
#include "core.hpp"

int main(int argc, char* argv[]) {

	const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
	};

	const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
	};

	Mesh meshA = Mesh(vertices, indices);

	auto core = Core::CoreManager();
	core.Initialize();
	core.getRenderer()->AddMesh(meshA);
	core.Run();

	core.Shutdown();
	return 0;
}
