//
// Created by Sebastian Borsch on 28.05.24.
//

#ifndef JAREP_METALDESCRIPTORBUILDER_HPP
#define JAREP_METALDESCRIPTORBUILDER_HPP

#include "IRenderAPI.hpp"
#include "MetalDescriptor.hpp"
#include "MetalDevice.hpp"

namespace Graphics::Metal {

	class MetalDescriptorBuilder final : public JarDescriptorBuilder {
		public:
			MetalDescriptorBuilder() = default;

			~MetalDescriptorBuilder() override;

			MetalDescriptorBuilder* SetBinding(uint32_t binding) override;

			MetalDescriptorBuilder* SetStageFlags(StageFlags stageFlags) override;

			std::shared_ptr <JarDescriptor> BuildUniformBufferDescriptor(std::shared_ptr <JarDevice> device,
			                                                             std::vector <std::shared_ptr<JarBuffer>> uniformBuffers) override;

			std::shared_ptr <JarDescriptor>
			BuildImageBufferDescriptor(std::shared_ptr <JarDevice> device, std::shared_ptr <JarImage> image) override;

		private:
			std::optional <uint32_t> m_binding;
			std::optional <StageFlags> m_stageFlags;
	};

}

#endif //JAREP_METALDESCRIPTORBUILDER_HPP
