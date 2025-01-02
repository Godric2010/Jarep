//
// Created by sebastian on 1/2/2025.
//

#pragma once

namespace JAREP::Window {

    /**
     * @enum DisplayMode
     * @brief Represents the mode the window is currently displayed in
     */
    enum class DisplayMode {
        /**
         * @brief A regular resizable window with border attributes.
         */
        BorderedWindow,
        /**
         * @brief A fullscreen window with no borders and no resize ability.
         */
        BorderlessWindow,
        /**
         * @brief A fullscreen window with no borders, keeping the focus of mouse and keyboard.
         */
        FullscreenWindow,
    };
}