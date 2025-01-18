//
// Created by sebastian on 1/13/2025.
//
#pragma once

#include<vulkan\vulkan.hpp>

namespace JAREP::Rendering {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual bool Initialize() = 0;

        virtual void Shutdown() = 0;
    };
}
