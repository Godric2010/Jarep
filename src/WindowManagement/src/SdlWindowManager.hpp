//
// Created by Sebastian Borsch on 31.12.24.
//

#pragma once
#define SDL_MAIN_HANDLED
#include <functional>
#include <optional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
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

        int32_t GetWindowWidth() override;

        int32_t GetWindowHeight() override;

        std::optional<IWindowHandle> GetNativeWindowHandle() override;


    private:
        SDL_Window *window = nullptr;
        int32_t window_width = 0;
        int32_t window_height = 0;
        int8_t display_index = 0;
        DisplayMode display_mode;

        std::vector<WindowUpdatedCallback> update_callbacks;

        bool closeRequested = false;
        bool isDirty = false;

        [[nodiscard]] std::optional<std::pair<int32_t, int32_t> > getWindowDimensions() const;
        void updateWindow();
        void setWindowFullscreenMode(std::pair<int32_t, int32_t> displayDimensions);
        void setWindowBorderlessWindowMode(std::pair<int32_t, int32_t> displayDimensions);
        void setWindowBorderedWindowMode(std::pair<int32_t, int32_t> displayDimensions);
    };
}
