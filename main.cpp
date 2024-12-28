//
// Created by sebastian on 12/23/2024.
//

#include <iostream>
#include "WindowManagement/WindowManagement.hpp"
int main() {
    std::cout << "Hello World!" << std::endl;

    auto window = WindowManagement();
    auto val   = window.foo(42);
    std::cout << val << std::endl;

    return 0;
}