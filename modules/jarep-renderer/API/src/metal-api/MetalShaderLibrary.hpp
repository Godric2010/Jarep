//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALSHADERLIBRARY_HPP
#define JAREP_METALSHADERLIBRARY_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {

	class MetalShaderLibrary final : public JarShaderModule {
		public:
			explicit MetalShaderLibrary(MTL::Library* library) : m_library(library) {
			}

			~MetalShaderLibrary() override;

			void Release() override;

			[[nodiscard]] MTL::Library* getLibrary() const { return m_library; }

		private:
			MTL::Library* m_library;
	};
}
#endif //JAREP_METALSHADERLIBRARY_HPP
