#include "SdlWindowManager.hpp"

using namespace JAREP::Window;

SDLWindowManager::SDLWindowManager() = default;

SDLWindowManager::~SDLWindowManager() = default;

bool SDLWindowManager::Initialize(WindowSettings display_settings) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return false;
	}

	Uint32 flags = SDL_WINDOW_VULKAN;

	window_width = display_settings.displayWidth;
	window_height = display_settings.displayHeight;
	window = SDL_CreateWindow(display_settings.windowTitle.c_str(),
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          window_width,
	                          window_height,
	                          flags);
	return window != nullptr;
}

void SDLWindowManager::PollEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			closeRequested = true;
		}
	}
}

bool SDLWindowManager::ShouldClose() {
	return closeRequested;
}

void SDLWindowManager::DestroyWindow() {
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}