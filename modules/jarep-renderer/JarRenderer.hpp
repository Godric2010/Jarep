//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_JARRENDERER_HPP
#define JAREP_JARRENDERER_HPP

#include <iostream>
#include <memory>
#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "JarRenderStep.hpp"
#include "API/src/IRendererAPI/IRenderAPI.hpp"


namespace Graphics {

	namespace Internal {
		class JarMesh {
			public:
				JarMesh(const Mesh& mesh, std::shared_ptr<JarBuffer> vertexBuffer, std::shared_ptr<JarBuffer> indexBuffer)
						: mesh(mesh), vertexBuffer(std::move(vertexBuffer)), indexBuffer(std::move(indexBuffer)) {}

				~JarMesh() = default;

				[[nodiscard]] std::shared_ptr<JarBuffer> getVertexBuffer() const { return vertexBuffer; }

				[[nodiscard]] std::shared_ptr<JarBuffer> getIndexBuffer() const { return indexBuffer; }

				[[nodiscard]] uint32_t getIndexLength() const { return mesh.getIndices().size(); }

				void Destroy() {
					indexBuffer->Release();
					vertexBuffer->Release();
				}

			private:
				Mesh mesh;
				std::shared_ptr<JarBuffer> vertexBuffer;
				std::shared_ptr<JarBuffer> indexBuffer;
		};

		class JarModelViewProjection{
			public:
				glm::mat4 model;
				glm::mat4 view;
				glm::mat4 projection;
		};
	}

	class JarRenderer {
		public:
			explicit JarRenderer(const std::vector<const char*>& extensionNames);

			~JarRenderer() = default;

			void Initialize(NativeWindowHandleProvider* nativeWindowHandle, uint32_t resolutionX, uint32_t resolutionY);

			void ResizeSurface(uint32_t width, uint32_t height);

			void ChangeResolution(uint32_t resX, uint32_t resY);

			void AddMesh(Mesh& mesh);

			void AddRenderStep(std::unique_ptr<JarRenderStepDescriptor> renderStepBuilder);

			void Render();

			void Shutdown();

		private:
			std::vector<const char*> extensions;
			std::string shaderFileType;
			std::shared_ptr<Backend> backend;
			std::shared_ptr<JarSurface> surface;
			std::shared_ptr<JarRenderTarget> renderTarget;
			std::shared_ptr<JarDevice> device;
			std::shared_ptr<JarCommandQueue> queue;
			std::vector<std::shared_ptr<Internal::JarRenderStep>> renderSteps;


			std::vector<std::shared_ptr<JarBuffer>> uniformBuffers;
			std::vector<std::shared_ptr<JarImage>> images;
			std::vector<std::shared_ptr<JarDescriptor>> descriptors;

			std::vector<Internal::JarMesh> meshes;

			int frameCounter = 0;


			[[nodiscard]] std::shared_ptr<JarShaderModule> createShaderModule(
					ShaderType shaderType, const std::string& shaderName) const;

			void prepareModelViewProjectionForFrame();

			static std::string readFile(const std::string& filename) {
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
#endif //JAREP_JARRENDERER_HPP
