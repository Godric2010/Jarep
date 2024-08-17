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
#include <SDL_vulkan.h>
#include <string>

#if defined (_WIN32)
#include <windows.h>
#endif

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

			[[nodiscard]] std::shared_ptr<Graphics::JarRenderer> getRenderer() const override;

		private:
			SDL_Window* window;
			std::vector<DisplayOpts> displayModes;
			int activeOptIndex;
			bool isDirty;
			Uint32 windowFlags;

			uint32_t m_currentWidth;
			uint32_t m_currentHeight;
			std::optional<std::pair<uint32_t, uint32_t>> m_pendingResize;
			std::chrono::time_point<std::chrono::steady_clock> m_lastResizeTime;
			bool m_resizeOccurred;
		    bool m_isResolutionHighRes;
		    bool m_changeResolution;

			std::shared_ptr<Graphics::JarRenderer> renderer;


			void HandleKeyDownEvent(const SDL_Event& event);
			static std::vector<DisplayOpts> getAvailableDisplayOpts();

			std::optional<SDL_DisplayMode> getDisplayModeFromOpts();

			[[nodiscard]] std::optional<NativeWindowHandleProvider*>
			getNativeWindowHandle(int sizeWidth, int sizeHeight) const;

			[[nodiscard]] std::vector<const char*> getVulkanWindowExtensionsCStr() const;


	};
}

#endif //JAREP_SDLWINDOW_HPP
