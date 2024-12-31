//
// Created by Sebastian Borsch on 31.12.24.
//

#pragma once
#include <SDL2/SDL.h>
#include "WindowManagement/IWindowManager.hpp"
#include "WindowManagement/WindowSettings.hpp"

namespace JAREP::Window {
	class SDLWindowManager final : public IWindowManager {
		public:
			SDLWindowManager();

			~SDLWindowManager() override;

			bool Initialize(WindowSettings display_settings) override;

			void PollEvents() override;

			bool ShouldClose() override;

			void DestroyWindow() override;

		private:
			SDL_Window* window = nullptr;
			int window_width = 0;
			int window_height = 0;
			bool closeRequested = false;
	};
}
