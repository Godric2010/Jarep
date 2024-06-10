//
// Created by sebastian on 09.06.24.
//

#ifndef JAREP_VULKANSURFACE_HPP
#define JAREP_VULKANSURFACE_HPP

#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include <vulkan/vulkan.hpp>

namespace Graphics::Vulkan {

	class VulkanDevice;

	struct SwapChainSupportDetails;
	class VulkanSwapchain;

	class VulkanSurface final : public JarSurface {
		public:
			VulkanSurface(VkSurfaceKHR surface, VkExtent2D surfaceExtend);

			~VulkanSurface() override;

			void RecreateSurface(uint32_t width, uint32_t height) override;

			void ReleaseSwapchain() override;

			uint32_t GetSwapchainImageAmount() override;

			JarExtent GetSurfaceExtent() override;

			void FinalizeSurface(std::shared_ptr<VulkanDevice> device);

			SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice) const;

			[[nodiscard]] VkSurfaceKHR getSurface() const { return m_surface; }

			[[nodiscard]] VkExtent2D getSurfaceExtent() const { return m_surfaceExtent; }

			[[nodiscard]] VulkanSwapchain* getSwapchain() const { return m_swapchain.get(); }

		private:
			VkSurfaceKHR m_surface;
			VkExtent2D m_surfaceExtent{};
			std::unique_ptr<VulkanSwapchain> m_swapchain;
			std::shared_ptr<VulkanDevice> m_device;
	};
}

#endif //JAREP_VULKANSURFACE_HPP
