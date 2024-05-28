//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALIMAGEBUILDER_HPP
#define JAREP_METALIMAGEBUILDER_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>
#include "MetalDevice.hpp"
#include "MetalImage.hpp"
#include "MetalCommandQueueBuilder.hpp"

namespace Graphics::Metal {
	class MetalImageBuilder final : public JarImageBuilder {
		public:
			MetalImageBuilder() = default;

			~MetalImageBuilder() override;

			MetalImageBuilder* SetPixelFormat(PixelFormat format) override;

			MetalImageBuilder* SetImagePath(std::string path) override;

			MetalImageBuilder* EnableMipMaps(bool enabled) override;

			std::shared_ptr<JarImage> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<MTL::PixelFormat> m_pixelFormat;
			std::optional<std::string> m_imagePath;
			bool m_enableMipMapping;

			static void generateMipMaps(std::shared_ptr<MetalDevice>& device, MTL::Texture* texture);
	};
}


#endif //JAREP_METALIMAGEBUILDER_HPP
