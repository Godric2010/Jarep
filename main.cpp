//
// Created by sebastian on 12/23/2024.
//

#include <chrono>
#include <iostream>

#include "WindowManagement/WindowCreator.hpp"

int main() {
    std::cout << "Hello World!" << std::endl;

    const auto window_manager = JAREP::Window::CreateWindowManager();
    const auto window_settings = JAREP::Window::WindowSettings{
        .windowTitle = "JAREP",
        .displayIndex = 1,
        .displayWidth = 800,
        .displayHeight = 600,
        .displayRefreshRate = 72
    };
    if (bool init_success = window_manager->Initialize(window_settings); !init_success) {
        std::cerr << "Failed to initialize window manager" << std::endl;
        return -1;
    }

    const auto window_settings_2 = JAREP::Window::WindowSettings{
        .windowTitle = "JAREP",
        .displayIndex = 1,
        .displayWidth = 1920,
        .displayHeight = 1080,
        .displayRefreshRate = 72
    };

    auto start_time = std::chrono::high_resolution_clock::now();
    while (!window_manager->ShouldClose()) {
        window_manager->PollEvents();
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta_time = current_time - start_time;
        if (delta_time.count() > 3) {
            window_manager->SetWindowSettings(window_settings_2);
        }
    }
    window_manager->DestroyWindow();

    return 0;
}
