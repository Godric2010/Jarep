//
// Created by sebastian on 1/13/2025.
//
#pragma once

#include<vulkan\vulkan.hpp>

#include "Core/RenderObject.hpp"
#include "../../src/meshes/VulkanMeshRegistry.hpp"
#include "Core/CameraObject.hpp"

namespace JAREP::Rendering {
	enum class SystemType {
		Windows,
		X11,
		Wayland,
	};

	struct RenderSettings {
		SystemType systemType;
		void* handle;
		void* display;

		std::vector<const char *> extensions;

		uint32_t width = 800;
		uint32_t height = 600;

		uint32_t renderWidth = width;
		uint32_t renderHeight = height;

		uint8_t msaa = 1;
	};

	class IRenderer {
		public:
			virtual ~IRenderer() = default;

			virtual bool Initialize(RenderSettings render_settings, Core::CameraConfig cameraConfig) = 0;

			virtual void Resize(uint32_t width, uint32_t height) = 0;

			virtual void SetRenderResolution(uint32_t resX, uint32_t resY) = 0;

			virtual void AddRenderObject(JAREP::Core::MeshID meshID, std::shared_ptr<JAREP::Core::Types::Mesh> mesh,
			                             Core::ObjectUBO objectData) = 0;

			virtual void UpdateRenderObject(Core::RenderObject renderObject) = 0;

			virtual void DrawFrame() = 0;

			virtual void Shutdown() = 0;
	};
}
