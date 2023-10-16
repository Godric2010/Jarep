//
// Created by Sebastian Borsch on 13.10.23.
//

#ifndef JAREP_SDLWINDOW_HPP
#define JAREP_SDLWINDOW_HPP

#include "IWindow.hpp"
#include <vector>
#include <unordered_map>
#include <optional>
#include <set>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

namespace Core::Window {

	class SdlWindow : public IWindow {

		public:
			SdlWindow();

			~SdlWindow() override;

			void Init(int windowWidth, int windowHeight) override;

			void Update() override;

			void Shutdown() override;

			std::vector<DisplayOpts> GetAvailableDisplayOpts() override;

			void SetDisplayOpts(int displayIndex, int resolutionIndex, int refreshRateIndex) override;

		private:
			SDL_Window* window;
			std::vector<DisplayOpts> displayModes;
			int activeOptIndex;
			bool isDirty;
			Uint32 windowFlags;
			int width;
			int height;
			static std::vector<DisplayOpts> getAvailableDisplayOpts();
			std::optional<SDL_DisplayMode> getDisplayModeFromOpts();
			void* getNativeWindowHandle();

			std::unique_ptr<Graphics::JarepGraphics> renderer;
	};
}

#endif //JAREP_SDLWINDOW_HPP
