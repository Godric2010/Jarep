//
// Created by Sebastian Borsch on 26.05.24.
//

#ifndef JAREP_METALDEVICE_HPP
#define JAREP_METALDEVICE_HPP

#include <optional>
#include <Metal/Metal.hpp>
#include "IRenderAPI.hpp"
#include "MetalDataTypeMaps.hpp"

namespace Graphics::Metal {
	class MetalDevice : public JarDevice {
		public:
			MetalDevice() { _device = std::nullopt; }

			~MetalDevice() override;

			void Initialize();

			void Release() override;

			uint32_t GetMaxUsableSampleCount() override;

			bool IsFormatSupported(PixelFormat format) override;

			[[nodiscard]] std::optional<MTL::Device*> getDevice() const;

		private:
			std::optional<MTL::Device*> _device;
	};
}
#endif //JAREP_METALDEVICE_HPP
