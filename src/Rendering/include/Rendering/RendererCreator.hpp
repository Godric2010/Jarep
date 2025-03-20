//
// Created by Godri on 3/16/2025.
//

#pragma once
#include "IRenderer.hpp"
#include "../src/RenderManager.hpp"

namespace JAREP::Rendering {
	class IRenderer;

    /**
	* Create a new instance of the render manager.
    * @return A pointer to the created render manager.
	*/
    inline IRenderer* CreateRenderer() {
      const auto render_manager = new RenderManager();
      return render_manager;
    }
}

