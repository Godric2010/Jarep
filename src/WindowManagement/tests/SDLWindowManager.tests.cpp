//
// Created by sebastian on 1/2/2025.
//

#define CATCH_CONFIG_MAIN
#include <iostream>
#include <catch2/catch_all.hpp>
#include "../src/SdlWindowManager.hpp"

using namespace JAREP::Window;

struct CallbackResult {
    int width;
    int height;
    DisplayMode mode;
};

CallbackResult cbResult;


TEST_CASE("SDLWindowManager: Window Settings and callbacks", "[SDLWindowManager]") {
    auto *window_manager = new SDLWindowManager();
    SECTION("Create Window Manager") {
        auto settings = WindowSettings();
        settings.displayHeight = 800;
        settings.displayWidth = 800;
        settings.displayIndex = 0;
        settings.displayMode = DisplayMode::BorderedWindow;
        settings.windowTitle = "Test Window";
        settings.displayRefreshRate = 72;

        const bool success = window_manager->Initialize(settings);
        REQUIRE(success);
    }
    const auto result = window_manager->RegisterForWindowUpdate(
        [](const int width, const int height, const DisplayMode mode) {
            cbResult = {width, height, mode};
        });
    REQUIRE(result);

    SECTION("Set Window Settings to new size") {
        WindowSettings settings;
        settings.displayWidth = 1000;
        settings.displayHeight = 600;
        settings.displayMode = DisplayMode::BorderedWindow;
        window_manager->SetWindowSettings(settings);
        window_manager->PollEvents();

        REQUIRE(cbResult.width == 1000);
        REQUIRE(cbResult.height == 600);
    }

    SECTION("Set Window size bigger than display") {
         WindowSettings settings;
        settings.displayWidth = 10000;
        settings.displayHeight = 10000;
        settings.displayMode = DisplayMode::BorderedWindow;
        window_manager->SetWindowSettings(settings);
        window_manager->PollEvents();

        REQUIRE(cbResult.width < 10000);
        REQUIRE(cbResult.height < 10000);
    }

    SECTION("Set Window to Borderless Fullscreen") {
        WindowSettings settings;
         // Setting the display width and height to a very low value so that the real display resolution is higher in any case
        settings.displayWidth = 100;
        settings.displayHeight = 100;
        settings.displayMode = DisplayMode::BorderlessWindow;
        window_manager->SetWindowSettings(settings);
        window_manager->PollEvents();

        REQUIRE(cbResult.mode == DisplayMode::BorderlessWindow);
        REQUIRE(cbResult.width != 100);
        REQUIRE(cbResult.height != 100);
    }

    SECTION("Set Window to Fullscreen") {
        WindowSettings settings;
         // Setting the display width and height to a very low value so that the real display resolution is higher in any case
        settings.displayWidth = 100;
        settings.displayHeight = 100;
        settings.displayMode = DisplayMode::FullscreenWindow;
        window_manager->SetWindowSettings(settings);
        window_manager->PollEvents();

        REQUIRE(cbResult.mode == DisplayMode::FullscreenWindow);
        REQUIRE(cbResult.width != 100);
        REQUIRE(cbResult.height != 100);
    }
}
