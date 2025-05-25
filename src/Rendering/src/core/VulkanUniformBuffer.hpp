//
// Created by Godri on 4/22/2025.
//

#pragma once
#include <memory>

#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"


namespace JAREP::Rendering::Core {
	template<typename T>
	class VulkanUniformBuffer {
		public:
			VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, size_t size);

			~VulkanUniformBuffer();

			void Update(const T* data, size_t size);

			VkBuffer getBuffer() const;

		private:
			std::unique_ptr<VulkanBuffer> m_buffer;
	};
}

#include "VulkanUniformBuffer.inl"
