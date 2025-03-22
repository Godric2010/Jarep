#include "SdlWindowManager.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>

using namespace JAREP::Window;

SDLWindowManager::SDLWindowManager() = default;

SDLWindowManager::~SDLWindowManager() = default;

bool SDLWindowManager::Initialize(WindowSettings display_settings) {
    auto SDL_InitSuccess = SDL_Init(SDL_INIT_VIDEO);
    if (SDL_InitSuccess != 0) {
        return false;
    }

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

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
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
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

int32_t SDLWindowManager::GetWindowWidth() {
    return window_width;
}

int32_t SDLWindowManager::GetWindowHeight() {
    return window_height;
}

std::vector<const char *> SDLWindowManager::GetExtensions() {
    unsigned int extensions_count = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr)) {
        throw std::runtime_error("SDL_Vulkan_GetInstanceExtensions could not get number of extensions.");
    }
    std::vector<const char *> extensions(extensions_count);
    if (!SDL_Vulkan_GetInstanceExtensions(window,&extensions_count, extensions.data())) {
        throw std::runtime_error("SDL_Vulkan_GetInstanceExtensions failed");
    }
    return extensions;
}


std::optional<IWindowHandle> SDLWindowManager::GetNativeWindowHandle() {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(window, &info)) {
#if defined (WIN32)
        return std::make_optional(
            static_cast<IWindowHandle>(WinWindowHandle(info.info.win.window, info.info.win.hinstance)));
#elif defined (__linux__)
        return std::make_optional(static_cast<IWindowHandle>(X11WindowHandle(info.info.x11.window, info.info.x11.display)));
#else
    return std::nullopt;
#endif
    }
    return std::nullopt;
}


// private functions
std::optional<std::pair<int32_t, int32_t> > SDLWindowManager::getWindowDimensions() const {
    auto *display_bounds = new SDL_Rect();
    const int resultCode = SDL_GetDisplayBounds(display_index, display_bounds);

    if (resultCode != 0) {
        std::cout << SDL_GetError() << std::endl;
        return std::nullopt;
    }

    std::pair<int32_t, int32_t> dimensions = std::make_pair(display_bounds->w, display_bounds->h);
    delete display_bounds;
    display_bounds = nullptr;

    return std::make_optional(dimensions);
}


void SDLWindowManager::updateWindow() {
    const auto display_dimensions = getWindowDimensions();
    if (!display_dimensions.has_value()) {
        throw std::exception("Display dimensions not found!");
    }
    switch (display_mode) {
        case DisplayMode::BorderedWindow:
            setWindowBorderedWindowMode(display_dimensions.value());
            break;
        case DisplayMode::FullscreenWindow:
            setWindowFullscreenMode(display_dimensions.value());
            break;
        case DisplayMode::BorderlessWindow:
            setWindowBorderlessWindowMode(display_dimensions.value());
            break;
    }

    std::ranges::for_each(update_callbacks, [&](auto &callback) {
        callback(window_width, window_height, display_mode);
    });
    isDirty = false;
}

void SDLWindowManager::setWindowFullscreenMode(const std::pair<int32_t, int32_t> displayDimensions) {
    window_width = displayDimensions.first;
    window_height = displayDimensions.second;
    SDL_SetWindowSize(window, window_width, window_height);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_SetWindowBordered(window, SDL_FALSE);
    SDL_SetWindowGrab(window, SDL_TRUE);
}

void SDLWindowManager::setWindowBorderlessWindowMode(const std::pair<int32_t, int32_t> displayDimensions) {
    window_width = displayDimensions.first;
    window_height = displayDimensions.second;

    SDL_SetWindowFullscreen(window, 0);
    SDL_SetWindowBordered(window, SDL_FALSE);
    SDL_SetWindowSize(window, window_width, window_height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowGrab(window, SDL_FALSE);
}

void SDLWindowManager::setWindowBorderedWindowMode(const std::pair<int32_t, int32_t> displayDimensions) {
    if (window_width > displayDimensions.first) {
        window_width = displayDimensions.first;
    }
    if (window_height > displayDimensions.second) {
        window_height = displayDimensions.second;
    }

    SDL_SetWindowFullscreen(window, 0);
    SDL_SetWindowBordered(window, SDL_TRUE);
    SDL_SetWindowSize(window, window_width, window_height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowGrab(window, SDL_FALSE);
}
