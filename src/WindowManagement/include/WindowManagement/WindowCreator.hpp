//
// Created by Sebastian Borsch on 31.12.24.
//

#pragma once

#include "IWindowManager.hpp"
#include "../src/SdlWindowManager.hpp"
#include <memory>

namespace JAREP::Window {
	class IWindowManager;
	/**
	 * Create a new instance of window manager
	 * @return A unique pointer of the created window manager
	 */
	inline IWindowManager* CreateWindowManager() {
		const auto sdl_window_manager = new SDLWindowManager();
		return sdl_window_manager;
	}


}
