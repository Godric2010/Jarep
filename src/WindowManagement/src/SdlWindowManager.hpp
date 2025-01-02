//
// Created by Sebastian Borsch on 31.12.24.
//

#pragma once
#define SDL_MAIN_HANDLED
#include <functional>
#include <SDL2/SDL.h>
#include "WindowManagement/IWindowManager.hpp"
#include "WindowManagement/WindowSettings.hpp"

namespace JAREP::Window {
	class SDLWindowManager final : public IWindowManager {
		public:
			SDLWindowManager();

			~SDLWindowManager() override;

			bool Initialize(WindowSettings display_settings) override;

			void SetWindowSettings(WindowSettings display_settings) override;

			std::expected<void, std::string> RegisterForWindowUpdate(WindowUpdatedCallback callback) override;

			void PollEvents() override;

			bool ShouldClose() override;

			void DestroyWindow() override;

		private:
			SDL_Window* window = nullptr;
			int window_width = 0;
			int window_height = 0;
			int display_index = 0;
			DisplayMode display_mode;

			std::vector<WindowUpdatedCallback> update_callbacks;

			bool closeRequested = false;
			bool isDirty = false;


			void updateWindow();
	};
}
