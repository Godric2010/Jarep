#include <iostream>
#include "modules/jarep-ecs/include/World.hpp"
#include "modules/jarep-renderer/Renderer.hpp"

int main() {

    std::cout << "Hello, World!" << std::endl;
//    World::test();
    auto renderer = new Renderer();
    renderer->test();

    return 0;
}
