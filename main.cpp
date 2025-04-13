//
// Created by sebastian on 12/23/2024.
//

#include <chrono>
#include <iostream>

#include "WindowManagement/WindowCreator.hpp"
#include "Rendering/RendererCreator.hpp"
#include "EngineCore/Types/Vertex.hpp"

bool framebufferResized = false;

int main() {
	JAREP::Core::Types::Vertex vert;

	const auto window_manager = JAREP::Window::CreateWindowManager();
	const auto window_settings = JAREP::Window::WindowSettings{
		.windowTitle = "JAREP",
		.displayIndex = 0,
		.displayWidth = 800,
		.displayHeight = 600,
		.displayRefreshRate = 72,
		.displayMode = JAREP::Window::DisplayMode::BorderlessWindow,
	};
	const auto window_settings_2 = JAREP::Window::WindowSettings{
		.windowTitle = "JAREP",
		.displayIndex = 0,
		.displayWidth = 1920,
		.displayHeight = 1080,
		.displayRefreshRate = 72,
		.displayMode = JAREP::Window::DisplayMode::BorderedWindow,
	};
	if (bool init_success = window_manager->Initialize(window_settings); !init_success) {
		std::cerr << "Failed to initialize window manager" << std::endl;
		return -1;
	}

	auto render_extensions = window_manager->GetExtensions();
	auto [handle, display, type] = window_manager->GetNativeWindowHandle();

	JAREP::Rendering::RenderSettings render_settings;
	render_settings.systemType = JAREP::Rendering::SystemType::Windows;
	render_settings.extensions = render_extensions;
	render_settings.handle = handle;
	render_settings.display = display;
	render_settings.width = window_settings.displayWidth;
	render_settings.height = window_settings.displayHeight;
	render_settings.msaa = 8;


	auto* renderer = JAREP::Rendering::CreateRenderer();
	renderer->Initialize(render_settings);


	auto result = window_manager->RegisterForWindowUpdate([](int width, int height, JAREP::Window::DisplayMode mode) {
		std::cout << "Width: " << width << ", Height: " << height << std::endl;
		framebufferResized = true;
	});


	auto start_time = std::chrono::high_resolution_clock::now();
	bool resized = false;
	while (!window_manager->ShouldClose()) {
		window_manager->PollEvents();
		auto current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta_time = current_time - start_time;
		if (delta_time.count() > 1 && !resized) {
			window_manager->SetWindowSettings(window_settings_2);
			resized = true;
			continue;
		}
		if (framebufferResized) {
			renderer->Resize(window_settings_2.displayWidth, window_settings_2.displayHeight);
			framebufferResized = false;
			continue;
		}

		renderer->DrawFrame();
	}
	window_manager->DestroyWindow();

	return 0;
}
