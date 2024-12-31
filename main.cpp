//
// Created by sebastian on 12/23/2024.
//

#include <iostream>

#include "WindowManagement/WindowCreator.hpp"

int main() {
	std::cout << "Hello World!" << std::endl;

	const auto window_manager = JAREP::Window::CreateWindowManager();
	const auto window_settings = JAREP::Window::WindowSettings{
		.windowTitle = "Hello World!",
		.displayIndex = 0,
		.displayWidth = 800,
		.displayHeight = 600,
		.displayRefreshRate = 72
	};
	window_manager->Initialize(window_settings);

	while (!window_manager->ShouldClose()) {
		window_manager->PollEvents();
	}
	window_manager->DestroyWindow();

	return 0;
}
