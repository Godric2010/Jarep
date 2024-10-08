//
// Created by Sebastian Borsch on 10.10.23.
//

#ifndef JAREP_CORE_HPP
#define JAREP_CORE_HPP

#include <iostream>
#include <memory>

#include "Window/IWindow.hpp"
#include "Window/sdlwindow.hpp"
#include "JarRenderer.hpp"
#include "JarRenderStep.hpp"


namespace Core {
	class CoreManager {
		public:
			CoreManager() = default;

			~CoreManager() = default;

			void Initialize();

			void Run();

			void Shutdown();

			[[nodiscard]] std::shared_ptr<Graphics::JarRenderer> getRenderer() const { return window->getRenderer(); }

		private:
			std::unique_ptr<Window::IWindow> window;
	};
}

#endif //JAREP_CORE_HPP
