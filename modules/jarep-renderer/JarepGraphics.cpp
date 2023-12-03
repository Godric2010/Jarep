//
// Created by Sebastian Borsch on 24.10.23.
//

#include "JarepGraphics.hpp"

namespace Graphics {
	JarepGraphics::JarepGraphics(const std::vector<const char *> &extensionNames) {
		extensions = extensionNames;
#if defined(__APPLE__) && defined(__MACH__)
		backend = std::make_shared<Metal::MetalBackend>(Metal::MetalBackend());
		std::cout << "Using metal renderer!" << std::endl;
#else
		backend = std::make_shared<Vulkan::VulkanBackend>(Vulkan::VulkanBackend(extensionNames));
		std::cout << "Using vulkan renderer!" << std::endl;
#endif
	}
}
