//
// Created by Sebastian Borsch on 22.05.24.
//

#ifndef JAREP_METALCOMMANDQUEUEBUILDER_HPP
#define JAREP_METALCOMMANDQUEUEBUILDER_HPP

#include "IRenderAPI.hpp"
#include "metalapi.hpp"
namespace Graphics::Metal {

	class MetalCommandQueueBuilder final : public JarCommandQueueBuilder {
		public:
			MetalCommandQueueBuilder() = default;

			~MetalCommandQueueBuilder() override;

			MetalCommandQueueBuilder* SetCommandBufferAmount(uint32_t commandBufferAmount) override;

			std::shared_ptr<JarCommandQueue> Build(std::shared_ptr<JarDevice> device) override;

			std::optional <uint32_t> getAmountOfCommandBuffers() const {
				return m_amountOfCommandBuffers;
			}

		private:
			std::optional<uint32_t> m_amountOfCommandBuffers;
			const uint32_t DEFAULT_COMMAND_BUFFER_COUNT = 3;
	};
}

#endif //JAREP_METALCOMMANDQUEUEBUILDER_HPP
