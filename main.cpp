#include <iostream>
#include "World.hpp"

int main() {

    std::cout << "Hello, World!" << std::endl;
    auto world = new World();
    world->createNewEntity();
    return 0;
}
