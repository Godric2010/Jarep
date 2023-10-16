//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_JAREPGRAPHICS_HPP
#define JAREP_JAREPGRAPHICS_HPP

#include <iostream>
#include <memory>
#include "IRenderer.hpp"

namespace Graphics {
	class JarepGraphics {
		public:
			enum class API {
				Vulkan, Metal
			};
			API currentAPI;

			JarepGraphics(API api) : currentAPI(api) {};

			~JarepGraphics() = default;

			void Initialize(void* nativeWindowHandle) {
				switch (currentAPI) {
					case API::Vulkan:
						std::cout << "Using vulkan renderer!" << std::endl;
						break;
					case API::Metal:
						std::cout << "Using metal renderer!" << std::endl;
						break;
				}
			}

			void Render() {
			}

			void Shutdown() {
				std::cout << "Shutdown renderer" << std::endl;
			}

		private:
			std::shared_ptr<IRenderer> renderAPI;


	};
}
#endif //JAREP_JAREPGRAPHICS_HPP
