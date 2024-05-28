//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALIMAGE_HPP
#define JAREP_METALIMAGE_HPP

#include "IRenderAPI.hpp"
#include <Metal/Metal.hpp>

namespace Graphics::Metal {

	class MetalImage final : public JarImage {
		public:
			MetalImage(MTL::Texture* texture, MTL::SamplerState* sampler) : m_texture(texture),
			                                                                m_samplerState(sampler) {}

			~MetalImage() override;

			void Release() override;

			[[nodiscard]] MTL::Texture* getTexture() const { return m_texture; }

			[[nodiscard]] MTL::SamplerState* getSampler() const { return m_samplerState; }

		private:
			MTL::Texture* m_texture;
			MTL::SamplerState* m_samplerState;
	};

}
#endif //JAREP_METALIMAGE_HPP
