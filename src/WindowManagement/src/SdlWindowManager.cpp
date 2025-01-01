#include "SdlWindowManager.hpp"

using namespace JAREP::Window;

SDLWindowManager::SDLWindowManager() = default;

SDLWindowManager::~SDLWindowManager() = default;

bool SDLWindowManager::Initialize(WindowSettings display_settings) {
	auto SDL_InitSuccess = SDL_Init(SDL_INIT_VIDEO);
	if (SDL_InitSuccess != 0) {
		std::cerr << "Failed to initialize SDL: " << SDL_InitSuccess << std::endl;
		return false;
	}
	std::cout << "Initializing SDL..." << std::endl;


	Uint32 flags =  SDL_WINDOW_SHOWN;

	window_width = display_settings.displayWidth;
	window_height = display_settings.displayHeight;
	window = SDL_CreateWindow( display_settings.windowTitle.c_str(),
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          display_settings.displayWidth,
	                          display_settings.displayHeight,
	                          flags);
	std::cout << "Initializing Window..." << std::endl;
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