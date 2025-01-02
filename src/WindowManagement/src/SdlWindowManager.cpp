#include "SdlWindowManager.hpp"

using namespace JAREP::Window;

SDLWindowManager::SDLWindowManager() = default;

SDLWindowManager::~SDLWindowManager() = default;

bool SDLWindowManager::Initialize(WindowSettings display_settings) {
	auto SDL_InitSuccess = SDL_Init(SDL_INIT_VIDEO);
	if (SDL_InitSuccess != 0) {
		return false;
	}


	Uint32 flags =  SDL_WINDOW_SHOWN;

	window_width = display_settings.displayWidth;
	window_height = display_settings.displayHeight;
	window = SDL_CreateWindow( display_settings.windowTitle.c_str(),
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          display_settings.displayWidth,
	                          display_settings.displayHeight,
	                          flags);
	return window != nullptr;
}

void SDLWindowManager::SetWindowSettings(WindowSettings display_settings) {
	window_width = display_settings.displayWidth;
	window_height = display_settings.displayHeight;
	isDirty = true;
}


void SDLWindowManager::PollEvents() {
	SDL_Event event;

	if (isDirty) {
		updateWindow();
	}

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


void SDLWindowManager::updateWindow() const {
	SDL_SetWindowSize(window, window_width, window_height);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
