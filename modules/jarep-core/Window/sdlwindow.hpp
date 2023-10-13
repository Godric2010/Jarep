//
// Created by Sebastian Borsch on 13.10.23.
//

#ifndef JAREP_SDLWINDOW_HPP
#define JAREP_SDLWINDOW_HPP

#include "IWindow.hpp"

namespace Core::Window {

	class SdlWindow : public IWindow {
		public:
			SdlWindow() = default;

			~SdlWindow() override = default;

			void Init() override;

			void Update() override;

			void Shutdown() override;
	};
}

#endif //JAREP_SDLWINDOW_HPP
