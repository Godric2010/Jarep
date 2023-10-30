//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_JAREPGRAPHICS_HPP
#define JAREP_JAREPGRAPHICS_HPP

#include <iostream>
#include <memory>
#include "IRenderer.hpp"
#include "metal-api/metalapi.hpp"

namespace Graphics {
	class JarepGraphics {
		public:
			enum class API {
				Vulkan, Metal
			};
			API currentAPI;

			JarepGraphics(API api) : currentAPI(api) {};

			~JarepGraphics() = default;

			void Initialize(void *nativeWindowHandle) {

				#if defined(__APPLE__) && defined(__MACH__)
				renderAPI = std::make_shared<Metal::MetalAPI>(Metal::MetalAPI());
				std::cout << "Using metal renderer!" << std::endl;
				#else
				std::cout << "Using vulkan renderer!" << std::endl;
				#endif

				renderAPI->CreateDevice();
				renderAPI->CreateSurface(nativeWindowHandle);
				renderAPI->CreateVertexBuffer();
				renderAPI->CreateShaders();
				renderAPI->CreateCommandQueue();
				renderAPI->CreateGraphicsPipeline();
			}

			void Render() {
				renderAPI->Draw();
			}

			void Shutdown() {
				std::cout << "Shutdown renderer" << std::endl;
			}

		private:
			std::shared_ptr<IRenderer> renderAPI;


	};
}
#endif //JAREP_JAREPGRAPHICS_HPP
