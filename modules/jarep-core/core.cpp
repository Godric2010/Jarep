//
// Created by Sebastian Borsch on 10.10.23.
//

#include "core.hpp"

namespace Core {

	void CoreManager::Initialize() {
		std::cout << "Initialize Core" << std::endl;
		window = std::make_unique<Window::SdlWindow>();
		window->Init(1920, 1080);

		auto windowOpts = window->GetAvailableDisplayOpts();
		for (const auto& opt: windowOpts) {
			printf("%s", opt.as_string());
		}

		auto renderStepDescriptor = std::make_unique<Graphics::JarRenderStepDescriptor>();
		renderStepDescriptor->m_vertexShaderName = std::string("triangle_vert");
		renderStepDescriptor->m_fragmentShaderName = "triangle_frag";
		renderStepDescriptor->m_depthTestEnabled = true;
		renderStepDescriptor->m_stencilTestEnabled = true;
		window->getRenderer()->AddRenderStep(std::move(renderStepDescriptor));
	}

	void CoreManager::Run() {
		std::cout << "Run Core" << std::endl;
		window->Update();
	}

	void CoreManager::Shutdown() {
		std::cout << "Shutdown Core" << std::endl;
		window->Shutdown();
	}
}
