//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_JAREPGRAPHICS_HPP
#define JAREP_JAREPGRAPHICS_HPP

#include <iostream>
#include <memory>
#include <filesystem>
#include <fstream>
#include <vector>
#include "IRenderer.hpp"
#include "NativeWindowHandleProvider.hpp"
#include "metal-api/metalapi.hpp"
#include "vulkan-api/vulkanapi.hpp"
#include "Vertex.hpp"

namespace Graphics
{
    class JarepGraphics
    {
    public:
        JarepGraphics(const std::vector<const char*>& extensionNames);

        ~JarepGraphics() = default;

        void Initialize(NativeWindowHandleProvider* nativeWindowHandle)
        {
            surface = backend->CreateSurface(nativeWindowHandle);
            device = backend->CreateDevice(surface);
            queue = device->CreateCommandQueue();

            const std::vector<Vertex> vertices = {
                {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
            };

            const size_t vertexDataSize = vertices.size() * sizeof(Vertex);
            vertexBuffer = device->CreateBuffer(vertexDataSize, vertices.data());
            vertexShaderModule = device->CreateShaderModule(readFile("shaders/triangle_vert.metal"));
            fragmentShaderModule = device->CreateShaderModule(readFile("shaders/triangle_frag.metal"));
            renderPass = device->CreateRenderPass(surface);
            pipeline = device->CreatePipeline(vertexShaderModule, fragmentShaderModule, renderPass);
        }

        void Render()
        {
            const auto commandBuffer = queue->getNextCommandBuffer();
            commandBuffer->StartRecording(surface, renderPass);

            commandBuffer->BindPipeline(pipeline);
            commandBuffer->BindVertexBuffer(vertexBuffer);
            commandBuffer->Draw();

            commandBuffer->EndRecording();
            commandBuffer->Present(surface, device);
        }

        void Shutdown()
        {
            pipeline->Release();
            vertexShaderModule->Release(device);
            fragmentShaderModule->Release(device);

            queue->Release(device);
            device->Release();

            std::cout << "Shutdown renderer" << std::endl;
        }

    private:
        std::vector<const char*> extensions;
        std::shared_ptr<Backend> backend;
        std::shared_ptr<JarSurface> surface;
        std::shared_ptr<JarDevice> device;
        std::shared_ptr<JarCommandQueue> queue;
        std::shared_ptr<JarBuffer> vertexBuffer;
        std::shared_ptr<JarShaderModule> vertexShaderModule;
        std::shared_ptr<JarShaderModule> fragmentShaderModule;
        std::shared_ptr<JarPipeline> pipeline;
        std::shared_ptr<JarRenderPass> renderPass;


        static std::string readFile(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!std::filesystem::exists(filename))
            {
                throw std::runtime_error("File does not exist: " + filename);
            }

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open file: " + filename);
            }

            auto fileSize = (size_t)file.tellg();
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
