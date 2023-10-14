//
// Created by Sebastian Borsch on 13.10.23.
//

#include "sdlwindow.hpp"

namespace Core::Window {
	void SdlWindow::Init(int width, int height) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			return;
		}

		#if defined(__APPLE__)
		windowFlags = SDL_WINDOW_METAL;
		#else
		windowFlags = SDL_WINDOW_VULKAN;
		#endif


		window = SDL_CreateWindow(
				"J.A.R.E.P",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				width, height,
				windowFlags
		);

		if (window == nullptr) {
			return;
		}
	}

	void SdlWindow::Update() {
		bool running = true;
		SDL_Event event;

		while (running) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = false;
				}
			}
		}
	}

	void SdlWindow::Shutdown() {
		SDL_Quit();
	}
}