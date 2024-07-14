//
// Created by Sebastian Borsch on 07.07.24.
//

#ifndef JAREP_METALIMAGEBUFFER_HPP
#define JAREP_METALIMAGEBUFFER_HPP

#include "IRenderAPI.hpp"
#include "MetalDevice.hpp"
#include "MetalCommandQueue.hpp"
#include <Metal/Metal.hpp>
#include <functional>
#include <memory>

namespace Graphics::Metal {

	class MetalCommandQueue;

	class MetalImageBuffer final : public JarImageBuffer {

		public:
			MetalImageBuffer(std::shared_ptr<MetalDevice> device, MTL::Texture* texture, std::function<std::shared_ptr<MetalCommandQueue>()> getCommandQueueCb);

			~MetalImageBuffer() override;

			void Release() override;

			void UploadData(const void* data, size_t bufferSize) override;

			MTL::Texture* GetTexture() const;

		private:
			MTL::Texture* m_texture;
			std::shared_ptr<MetalDevice> m_device;
			std::function<std::shared_ptr<MetalCommandQueue>()> m_getCommandQueueCb;

	};
}

#endif //JAREP_METALIMAGEBUFFER_HPP
