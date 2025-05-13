//
// Created by sebastian on 12/23/2024.
//

#include <chrono>
#include <iostream>

#include "EngineCore/Types/Transform.hpp"
#include "WindowManagement/WindowCreator.hpp"
#include "Rendering/RendererCreator.hpp"
#include "src/AssetLoader/src/MeshLoader.hpp"
#include "src/EngineCore/src/MeshLibrary.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "EngineCore/Camera.hpp"

bool framebufferResized = false;


JAREP::Rendering::Core::InstanceData createUBO(const JAREP::Core::Types::Transform&transform) {
	glm::mat4 transformMatrix = transform.ToMatrix();

	JAREP::Rendering::Core::InstanceData ubo{};
	std::memcpy(ubo.modelMatrix.data(), glm::value_ptr(transformMatrix), sizeof(glm::mat4));
	return ubo;
}

JAREP::Rendering::Core::CameraUBO createCameraUBO(const JAREP::Core::Camera&camera, const float renderWidth,
                                                  const float renderHeight) {
	const glm::mat4 world = camera.transform.ToMatrix();
	glm::mat4 view = glm::inverse(world);
	glm::mat4 projection;
	if (camera.isOrthographic) {
		const float w = camera.orthographicWidth * 0.5;
		const float h = camera.orthographicHeight * 0.5;
		projection = glm::ortho(-w, w, -h, h, camera.nearZ, camera.farZ);
	}
	else {
		const float aspectRatio = renderWidth / renderHeight;
		projection = glm::perspectiveRH_ZO(glm::radians(camera.fovY), aspectRatio, camera.nearZ, camera.farZ);
	}

	JAREP::Rendering::Core::CameraUBO ubo{};
	std::memcpy(ubo.viewMatrix.data(), glm::value_ptr(view), sizeof(glm::mat4));
	std::memcpy(ubo.projectionMatrix.data(), glm::value_ptr(projection), sizeof(glm::mat4));
	ubo.isOrthographic = camera.isOrthographic ? 1u : 0u;
	return ubo;
}

JAREP::Rendering::Core::CameraConfig createCameraConfig(const JAREP::Core::Camera&camera, const float renderWidth,
                                                        const float renderHeight) {
	JAREP::Rendering::Core::CameraConfig cameraConfig{};
	cameraConfig.cameraUBO = createCameraUBO(camera, renderWidth, renderHeight);
	cameraConfig.nearPlane = camera.nearZ;
	cameraConfig.farPlane = camera.farZ;
	cameraConfig.cullBackFaces = camera.cullBackFaces;
	cameraConfig.cullFrontFaces = camera.cullFrontFaces;
	return cameraConfig;
}

int main() {
	auto meshLoader = JAREP::AssetLoader::MeshLoader();
	auto meshName = std::string("demo_cube");
	auto mesh = meshLoader.LoadMeshAsObj(meshName);

	auto meshLibrary = JAREP::Core::MeshLibrary();
	auto meshID = meshLibrary.AddMesh(mesh);

	std::vector<JAREP::Core::Types::Transform> meshTransforms;

	for (int x = -5; x < 6; x += 5) {
		for (int y = -5; y < 6; y += 5) {
			JAREP::Core::Types::Transform transform{
				.position = {x, y, 0},
				.rotation = {0, 0, 0},
				.scale = {1, 1, 1},
			};
			meshTransforms.push_back(transform);
		}
	}

	JAREP::Core::Types::Transform meshTransform = {
		.position = {0, 0, 0},
		.rotation = {10, 10, 10},
		.scale = {1, 1, 1},
	};

	JAREP::Core::Camera camera{};
	camera.transform.position = {0, 0, 5};
	camera.transform.rotation = {0, 0, 0};
	camera.transform.scale = {1, 1, 1};
	camera.fovY = 75;
	camera.nearZ = 0.001f;
	camera.farZ = 1000.0f;
	camera.isOrthographic = false;
	camera.cullBackFaces = true;
	camera.cullFrontFaces = false;


	const auto window_manager = JAREP::Window::CreateWindowManager();
	const auto window_settings = JAREP::Window::WindowSettings{
		.windowTitle = "JAREP",
		.displayIndex = 0,
		.displayWidth = 800,
		.displayHeight = 600,
		.displayRefreshRate = 72,
		.displayMode = JAREP::Window::DisplayMode::BorderlessWindow,
	};
	const auto window_settings_2 = JAREP::Window::WindowSettings{
		.windowTitle = "JAREP",
		.displayIndex = 0,
		.displayWidth = 1920,
		.displayHeight = 1080,
		.displayRefreshRate = 72,
		.displayMode = JAREP::Window::DisplayMode::BorderedWindow,
	};
	if (bool init_success = window_manager->Initialize(window_settings); !init_success) {
		std::cerr << "Failed to initialize window manager" << std::endl;
		return -1;
	}

	auto render_extensions = window_manager->GetExtensions();
	auto [handle, display, type] = window_manager->GetNativeWindowHandle();

	JAREP::Rendering::RenderSettings renderSettings;
	renderSettings.systemType = JAREP::Rendering::SystemType::Windows;
	renderSettings.extensions = render_extensions;
	renderSettings.handle = handle;
	renderSettings.display = display;
	renderSettings.width = window_settings.displayWidth;
	renderSettings.height = window_settings.displayHeight;
	renderSettings.msaa = 8;

	JAREP::Rendering::Core::CameraConfig cameraConfig = createCameraConfig(
		camera, renderSettings.width, renderSettings.height);

	auto* renderer = JAREP::Rendering::CreateRenderer();
	renderer->Initialize(renderSettings, cameraConfig);

	JAREP::Rendering::Core::InstanceData transformUBO = createUBO(meshTransform);

	renderer->AddRenderObject(meshID, meshLibrary.GetMesh(meshID), transformUBO);

	auto result = window_manager->RegisterForWindowUpdate([](int width, int height, JAREP::Window::DisplayMode mode) {
		std::cout << "Width: " << width << ", Height: " << height << std::endl;
		framebufferResized = true;
	});


	auto start_time = std::chrono::high_resolution_clock::now();
	auto last_frame_time = start_time;
	bool resized = false;

	// FPS stuff
	float fpsAccumulator = 0.0f;
	int fpsFrameCount = 0;
	const float fpsDisplayRate = 1.0;
	while (!window_manager->ShouldClose()) {
		window_manager->PollEvents();
		auto current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta_time = current_time - start_time;
		if (delta_time.count() > 1 && !resized) {
			window_manager->SetWindowSettings(window_settings_2);
			resized = true;
			continue;
		}
		if (framebufferResized) {
			renderer->Resize(window_settings_2.displayWidth, window_settings_2.displayHeight);
			framebufferResized = false;
			continue;
		}

		float dt = std::chrono::duration<float>(current_time - last_frame_time).count();
		last_frame_time = current_time;

		float deltaDeg = 15.0f * dt;
		meshTransform.rotation.x += deltaDeg;
		meshTransform.rotation.y += deltaDeg;
		meshTransform.rotation.z += deltaDeg;

		transformUBO = createUBO(meshTransform);
		JAREP::Rendering::Core::RenderObject renderObject{
			.meshID = meshID,
			.transform = transformUBO,
		};
		renderer->UpdateRenderObject(renderObject);
		renderer->DrawFrame();

		fpsAccumulator += dt;
		fpsFrameCount++;
		if (fpsAccumulator >= fpsDisplayRate) {
			float fps = fpsFrameCount / fpsAccumulator;
			std::cout << "FPS: " << fps << std::endl;
			fpsAccumulator = 0.0;
			fpsFrameCount = 0;
		}
	}
	window_manager->DestroyWindow();

	return 0;
}
