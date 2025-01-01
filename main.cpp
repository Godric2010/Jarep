//
// Created by sebastian on 12/23/2024.
//

#include <iostream>

#include "WindowManagement/WindowCreator.hpp"

int main() {
	std::cout << "Hello World!" << std::endl;

	const auto window_manager = JAREP::Window::CreateWindowManager();
	const auto window_settings = JAREP::Window::WindowSettings{
		.windowTitle = "JAREP",
		.displayIndex = 1,
		.displayWidth = 900,
		.displayHeight = 600,
		.displayRefreshRate = 72
	};
	if (bool init_success =	window_manager->Initialize(window_settings); !init_success) {
		std::cerr << "Failed to initialize window manager" << std::endl;
		return -1;
	}

	while (!window_manager->ShouldClose()) {
		window_manager->PollEvents();
	}
	window_manager->DestroyWindow();

	return 0;
}
