//
// Created by Sebastian Borsch on 10.10.23.
//

#include "core.hpp"

namespace Core {

	void CoreManager::Initialize() {
		std::cout << "Initialize Core" << std::endl;
		window = std::make_unique<Window::SdlWindow>();
		window->Init();
	}

	void CoreManager::Run() {
		std::cout << "Run Core" << std::endl;
	}

	void CoreManager::Shutdown() {
		std::cout << "Shutdown Core" << std::endl;
	}
}
