//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_JAREPGRAPHICS_HPP
#define JAREP_JAREPGRAPHICS_HPP

#include <iostream>
#include <memory>
#include "IRenderer.hpp"
#include "NativeWindowHandleProvider.hpp"
#include "metal-api/metalapi.hpp"
#include "vulkan-api/vulkanapi.hpp"

namespace Graphics {
	class JarepGraphics {
		public:
			JarepGraphics(const std::vector<const char *> &extensionNames) : extensionNames(extensionNames) {
#if defined(__APPLE__) && defined(__MACH__)
				renderAPI = std::make_shared<Metal::MetalAPI>(Metal::MetalAPI());
				std::cout << "Using metal renderer!" << std::endl;
#else
				renderAPI = std::make_shared<Vulkan::VulkanAPI>(Vulkan::VulkanAPI(extensionNames));
				std::cout << "Using vulkan renderer!" << std::endl;
#endif
			}

			~JarepGraphics() = default;

			void Initialize(NativeWindowHandleProvider *nativeWindowHandle) {
				renderAPI->CreateSurface(nativeWindowHandle);
				renderAPI->RegisterPhysicalDevice();
				renderAPI->CreateLogicalDevice();
				renderAPI->CreateVertexBuffer();
				renderAPI->CreateShaders();
				renderAPI->CreateCommandQueue();
				renderAPI->CreateGraphicsPipeline();
			}

			void Render() {
			//	renderAPI->Draw();
			}

			void Shutdown() {
				std::cout << "Shutdown renderer" << std::endl;
			}

		private:
			std::vector<const char *> extensionNames;
			std::shared_ptr<IRenderer> renderAPI;
	};
}
#endif //JAREP_JAREPGRAPHICS_HPP
