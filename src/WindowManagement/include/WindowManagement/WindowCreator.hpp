//
// Created by Sebastian Borsch on 31.12.24.
//

#pragma once

#include "IWindowManager.hpp"
#include "../src/SdlWindowManager.hpp"

namespace JAREP::Window {
	class IWindowManager;
	/**
	 * Create a new instance of window manager
	 * @return A unique pointer of the created window manager
	 */
	inline std::unique_ptr<IWindowManager> CreateWindowManager()
	{
		return std::unique_ptr<SDLWindowManager>();
	}


}
