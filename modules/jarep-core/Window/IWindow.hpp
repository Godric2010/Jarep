//
// Created by Sebastian Borsch on 10.10.23.
//

#ifndef JAREP_IWINDOW_HPP
#define JAREP_IWINDOW_HPP

#include <iostream>

namespace Core::Window {
	class IWindow {

		public:
			virtual ~IWindow() = default;

			virtual void Init() = 0;

			virtual void Update() = 0;

			virtual void Shutdown() = 0;
	};
}
#endif //JAREP_IWINDOW_HPP
