//
// Created by Sebastian Borsch on 27.12.24.
//

#pragma once
#include "WindowSettings.hpp"

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
			 * @brief Constructor of the Window Management
			 */
			IWindowManager();

			/**
			 * @brief Destructor of the Window Management
			 */
			virtual ~IWindowManager();


			/**
			 * @brief Initialize a new window.
			 * @param display_settings Settings of this window.
			 * @return True if window was created successful.
			 */
			virtual bool Initialize(WindowSettings display_settings);

			/**
			 * @brief Poll the events from this window if its focussed.
			 */
			virtual void PollEvents();

			/**
			 * Evaluates if the window has been called to close
			 * @return True if the window is about to be destroyed, False if not.
			 */
			virtual bool ShouldClose();

			/**
			 * Destroy the window and remove it from memory.
			 */
			virtual void DestroyWindow();
	};
}
