//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALDESCRIPTORLAYOUT_HPP
#define JAREP_METALDESCRIPTORLAYOUT_HPP

#include "IRenderAPI.hpp"

namespace Graphics::Metal{

	class MetalDescriptorLayout final : public JarDescriptorLayout {
		public:
			MetalDescriptorLayout() = default;

			~MetalDescriptorLayout() override;

			void Release() override;

	};
}


#endif //JAREP_METALDESCRIPTORLAYOUT_HPP
