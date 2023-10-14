//
// Created by Sebastian Borsch on 13.10.23.
//

#ifndef JAREP_SDLWINDOW_HPP
#define JAREP_SDLWINDOW_HPP

#include "IWindow.hpp"
#include <SDL2/SDL.h>

namespace Core::Window {

	class SdlWindow : public IWindow {
		public:
			SdlWindow() = default;

			~SdlWindow() override = default;

			void Init(int width, int height) override;

			void Update() override;

			void Shutdown() override;

		private:
			SDL_Window* window;
	};
}

#endif //JAREP_SDLWINDOW_HPP
