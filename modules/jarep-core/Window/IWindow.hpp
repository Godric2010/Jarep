//
// Created by Sebastian Borsch on 10.10.23.
//

#ifndef JAREP_IWINDOW_HPP
#define JAREP_IWINDOW_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "JarepGraphics.hpp"

namespace Core::Window {

	struct DisplayOpts {
		public:
			int Index;
			const char *Name;
			std::vector<std::pair<int, int>> Resolutions;
			std::vector<int> RefreshRates;

			void SelectResolutionAndRefreshRate(int resolutionIndex, int refreshRateIndex) {
				selectedRefreshRateIndex = refreshRateIndex;
				selectedResolutionIndex = resolutionIndex;
			}

			std::pair<int, int> GetCurrentResolution() {
				return Resolutions[selectedResolutionIndex];
			}

			int GetCurrentRefreshRate() {
				return RefreshRates[selectedRefreshRateIndex];
			}

			const char *as_string() const {
				std::stringstream ss;
				ss << "Display: " << Name << " [" << Index << "]\n";
				ss << "  Resolutions:\n";
				for (const auto &res: Resolutions) {
					ss << "   (" << res.first << "," << res.second << ")\n";
				}
				ss << "\n  Refresh Rates:\n";
				for (const auto &rate: RefreshRates) {
					ss << "   " << rate << "Hz\n";
				}
				static std::string s = ss.str();
				return s.c_str();
			}

		private:
			int selectedResolutionIndex = 0;
			int selectedRefreshRateIndex = 0;

	};

	class IWindow {

		public:
			virtual ~IWindow() = default;

			virtual void Init(int width, int height) = 0;

			virtual std::vector<DisplayOpts> GetAvailableDisplayOpts() = 0;

			virtual void SetDisplayOpts(int displayIndex, int resolutionIndex, int refreshRateIndex) = 0;

			virtual void Update() = 0;

			virtual void Shutdown() = 0;

			[[nodiscard]] virtual std::shared_ptr<Graphics::JarepGraphics> getRenderer() const = 0;
	};

}
#endif //JAREP_IWINDOW_HPP
