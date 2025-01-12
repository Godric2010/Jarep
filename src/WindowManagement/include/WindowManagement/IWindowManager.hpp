//
// Created by Sebastian Borsch on 27.12.24.
//

#pragma once
#include "WindowSettings.hpp"
#include <expected>
#include <functional>

#include "IWindowHandle.hpp"

namespace JAREP::Window {
    /**
     * @class IWindowManager
     * @brief Main interface with the window management system
     *
     * Provides core functionality to the library.
     */
    class IWindowManager {
    public:
        /**
         *  @brief Callback when window settings has been updated.
         */
        using WindowUpdatedCallback = std::function<void(int width, int height, DisplayMode mode)>;

        /**
         * @brief Destructor of the Window Management
         */
        virtual ~IWindowManager() = default;

        /**
         * @brief Initialize a new window.
         * @param display_settings Settings of this window.
         * @return True if window was created successful.
         */
        virtual bool Initialize(WindowSettings display_settings) = 0;

        /**
         * @brief Set new display settings
         * @param display_settings The new settings that will be applied to the window.
         */
        virtual void SetWindowSettings(WindowSettings display_settings) = 0;

        /**
         * @brief Register for the window update event
         * @param callback callback function to execute on window update
         */
        virtual std::expected<void, std::string> RegisterForWindowUpdate(WindowUpdatedCallback callback) = 0;

        /**
         * @brief Poll the events from this window if its focussed.
         */
        virtual void PollEvents() = 0;

        /**
         * @brief Evaluates if the window has been called to close
         * @return True if the window is about to be destroyed, False if not.
         */
        virtual bool ShouldClose() = 0;

        /**
         * @brief Destroy the window and remove it from memory.
         */
        virtual void DestroyWindow() = 0;

        /**
         * @brief Get the current width of the window.
         * @return The window width in pixels.
         */
        virtual int32_t GetWindowWidth() = 0;

        /**
         * @brief Get the current height of the window.
         * @return The window height in pixels.
         */
        virtual int32_t GetWindowHeight() = 0;

        /**
         * @brief Get the window handle of this window.
         * @return Can be null-opt if platform is invalid. Returns valid IWindowHandle if platform is supported. Cast to platform handle for further usage.
         */
        virtual std::optional<IWindowHandle> GetNativeWindowHandle() = 0;
    };
}
