# README

**Version 0.1.0**

The Window Management Library provides a variety of customizations, callbacks and functions to have
simple access layer to the operating system, via the window itself.

Currently, Windows and Linux X11 platforms are supported. Support for Wayland is planned, macOS support is not planned
yet.
Underneath the hood, SDL2 is working with the system.

For a detailed overview over all functionalities, consult the API.md.

## Get started

To create window, you can simply call the *CreateWindowManager* function from the WindowCreator.hpp file.
Add some configurations and you are good to go!

````c++    
#include "WindowManagement/WindowCreator.hpp"  
    
int main() {
    const auto window_manager = JAREP::Window::CreateWindowManager();
    const auto window_settings = JAREP::Window::WindowSettings{
        .windowTitle = "MyName",
        .displayIndex = 0,
        .displayWidth = 800,
        .displayHeight = 600,
        .displayRefreshRate = 72,
        .displayMode = JAREP::Window::DisplayMode::BorderlessWindow,
    };
    
    if (bool init_success = window_manager->Initialize(window_settings); !init_success) {
        std::cerr << "Failed to initialize window manager" << std::endl;
        return -1;
    }

    while (!window_manager->ShouldClose()) {
        window_manager->PollEvents();
    }
    
    window_manager->DestroyWindow();

    return 0;
}
````