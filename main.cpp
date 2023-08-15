#include <iostream>
#include "world.hpp"

int main() {

    std::cout << "Hello, World!" << std::endl;
    auto world = new World();
    world->createNewEntity();
    return 0;
}
