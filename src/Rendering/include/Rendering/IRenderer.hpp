//
// Created by sebastian on 1/13/2025.
//
#pragma once

#include<vulkan\vulkan.hpp>

namespace JAREP::Rendering {
    enum class SystemType {
        Windows,
        X11,
        Wayland,
    };

    struct RenderSettings {
        SystemType systemType;
        void *handle;
        void *display;

        std::vector<const char *> extensions;

        uint32_t width = 800;
        uint32_t height = 600;

        uint32_t renderWidth = width;
        uint32_t renderHeight = height;
    };

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual bool Initialize(RenderSettings render_settings) = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void SetRenderResolution(uint32_t resX, uint32_t resY) = 0;

        virtual void DrawFrame() = 0;

        virtual void Shutdown() = 0;
    };
}
