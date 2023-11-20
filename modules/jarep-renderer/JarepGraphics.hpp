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
			JarepGraphics(const std::vector<const char *> &extensionNames);
			~JarepGraphics() = default;

			void Initialize(NativeWindowHandleProvider *nativeWindowHandle) {

				surface = backend->CreateSurface(nativeWindowHandle);
				device = backend->CreateDevice(surface);
				queue = device->CreateCommandQueue();

				// renderAPI->CreateSurface(nativeWindowHandle);
				// renderAPI->RegisterPhysicalDevice();
				// renderAPI->CreateLogicalDevice();
				// renderAPI->CreateVertexBuffer();
				// renderAPI->CreateShaders();
				// renderAPI->CreateCommandQueue();
				// renderAPI->CreateGraphicsPipeline();
			}

			void Render() {
				const auto commandBuffer = queue->getNextCommandBuffer();
				const auto renderPassDesc = surface->CreateRenderPass();
				commandBuffer->StartRecording(renderPassDesc);


				commandBuffer->EndRecording();
				commandBuffer->Present(surface);

			//	renderAPI->Draw();
			}

			void Shutdown() {
				queue->Release();
				device->Release();
				std::cout << "Shutdown renderer" << std::endl;
			}

		private:
			std::vector<const char *> extensions;
			std::shared_ptr<IRenderer> renderAPI;
			std::shared_ptr<Backend> backend;
			std::shared_ptr<JarSurface> surface;
			std::shared_ptr<JarDevice> device;
			std::shared_ptr<JarCommandQueue> queue;
	};
}
#endif //JAREP_JAREPGRAPHICS_HPP
