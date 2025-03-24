//
// Created by Sebastian Borsch on 31.12.24.
//

#pragma once
#include <string>

#include "DisplayMode.hpp"

namespace JAREP::Window {



	/**
	 * @struct WindowSettings
	 * @brief Contains all settings of the current window.
	 */
	struct WindowSettings {
		/**
		 * The title of the window.
		 */
		std::string windowTitle;

		/**
		 * The index of the display the window is shown at. 0 by default,
		 * which brings the window to the main screen.
		 */
		int8_t displayIndex = 0;

		/**
		 * The pixel resolution width of this window.
		 */
		int32_t displayWidth = 0;

		/**
		 * The pixel resolution height of this window.
		 */
		int32_t displayHeight = 0;

		/**
		 * The refresh rate of this window in hertz.
		 */
		int32_t displayRefreshRate = 0;

		/**
		 * The mode the display has to be set to.
		 */
		DisplayMode displayMode = DisplayMode::BorderedWindow;
	};
}
