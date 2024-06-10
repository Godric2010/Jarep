//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANCOMMANDQUEUEBUILDER_HPP
#define JAREP_VULKANCOMMANDQUEUEBUILDER_HPP

#include "IRenderAPI.hpp"
#include "VulkanCommandQueue.hpp"
#include <vulkan/vulkan.hpp>
#include <optional>
#include <memory>

namespace Graphics::Vulkan {
	class VulkanCommandQueueBuilder final : public JarCommandQueueBuilder {
		public:
			VulkanCommandQueueBuilder() = default;

			~VulkanCommandQueueBuilder() override;

			VulkanCommandQueueBuilder* SetCommandBufferAmount(uint32_t commandBufferAmount) override;

			std::shared_ptr <JarCommandQueue> Build(std::shared_ptr <JarDevice> device) override;

		private:
			std::optional <uint32_t> m_amountOfCommandBuffers;
			const uint32_t DEFAULT_COMMAND_BUFFER_COUNT = 3;
	};
}


#endif //JAREP_VULKANCOMMANDQUEUEBUILDER_HPP
