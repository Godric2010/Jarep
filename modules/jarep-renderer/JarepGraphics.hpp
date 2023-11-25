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
#include "Vertex.hpp"

namespace Graphics {
	class JarepGraphics {
		public:
			JarepGraphics(const std::vector<const char *> &extensionNames);
			~JarepGraphics() = default;

			void Initialize(NativeWindowHandleProvider *nativeWindowHandle) {

				surface = backend->CreateSurface(nativeWindowHandle);
				device = backend->CreateDevice(surface);
				queue = device->CreateCommandQueue();

				const std::vector<Vertex> vertices = {
						{{0.0f,  -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
						{{0.5f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
						{{-0.5f, 0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}}
				};

				const size_t vertexDataSize = vertices.size() * sizeof(Vertex);
				auto testBuffer = device->CreateBuffer(vertexDataSize, vertices.data());
				auto testShader = device->CreateShaderModule(readFile("shaders/triangle_vert.metal"));

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

			std::string readFile(const std::string &filename) {
				std::ifstream file(filename, std::ios::ate | std::ios::binary);

				if (!std::filesystem::exists(filename)) {
					throw std::runtime_error("File does not exist: " + filename);
				}

				if (!file.is_open()) {
					throw std::runtime_error("Failed to open file: " + filename);
				}

				auto fileSize = (size_t) file.tellg();
				std::vector<char> buffer(fileSize);

				file.seekg(0);
				file.read(buffer.data(), fileSize);

				file.close();
				std::string bufferString(buffer.begin(), buffer.end());
				return bufferString;
			}
	};
}
#endif //JAREP_JAREPGRAPHICS_HPP
