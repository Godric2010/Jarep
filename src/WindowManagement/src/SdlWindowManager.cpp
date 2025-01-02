#include "SdlWindowManager.hpp"

#include <iostream>
#include <ostream>
#include <bits/ranges_algo.h>

using namespace JAREP::Window;

SDLWindowManager::SDLWindowManager() = default;

SDLWindowManager::~SDLWindowManager() = default;

bool SDLWindowManager::Initialize(WindowSettings display_settings) {
    auto SDL_InitSuccess = SDL_Init(SDL_INIT_VIDEO);
    if (SDL_InitSuccess != 0) {
        return false;
    }


    Uint32 flags = SDL_WINDOW_SHOWN;

    window_width = display_settings.displayWidth;
    window_height = display_settings.displayHeight;
    display_mode = display_settings.displayMode;
    display_index = display_settings.displayIndex;
    window = SDL_CreateWindow(display_settings.windowTitle.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              display_settings.displayWidth,
                              display_settings.displayHeight,
                              flags);
    if (window == nullptr) {
        return false;
    }
    updateWindow();
    return true;
}

void SDLWindowManager::SetWindowSettings(WindowSettings display_settings) {
    window_width = display_settings.displayWidth;
    window_height = display_settings.displayHeight;
    display_mode = display_settings.displayMode;
    display_index = display_settings.displayIndex;
    isDirty = true;
}

std::expected<void, std::string> SDLWindowManager::RegisterForWindowUpdate(WindowUpdatedCallback callback) {
    if (!callback) {
        return std::unexpected("Invalid callback!");
    }
    update_callbacks.push_back(std::move(callback));
    return {};
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


void SDLWindowManager::updateWindow() {
    switch (display_mode) {
        case DisplayMode::BorderedWindow:
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowBordered(window, SDL_TRUE);
            SDL_SetWindowSize(window, window_width, window_height);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
        case DisplayMode::FullscreenWindow:
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_SetWindowBordered(window, SDL_FALSE);
            SDL_SetWindowGrab(window, SDL_TRUE);
            break;
        case DisplayMode::BorderlessWindow:
            auto *display_bounds = new SDL_Rect();
            int result = SDL_GetDisplayBounds(display_index, display_bounds);
            if (result != 0) {
                std::cout << SDL_GetError() << std::endl;
            } else {
                window_width = display_bounds->w;
                window_height = display_bounds->h;
            }
            delete display_bounds;
            display_bounds = nullptr;


            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowBordered(window, SDL_FALSE);
            SDL_SetWindowSize(window, window_width, window_height);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            SDL_SetWindowGrab(window, SDL_FALSE);
            break;
    }

    std::ranges::for_each(update_callbacks, [&](auto &callback) {
        callback(window_width, window_height, display_mode);
    });
    isDirty = false;
}
