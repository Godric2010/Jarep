//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_VULKANBACKEND_HPP
#define JAREP_VULKANBACKEND_HPP
#if defined(__linux__) || defined(_WIN32)

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include "sdlsurfaceadapter.hpp"

#if defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR

#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_wayland.h>

#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef BUILDING_DLL
#define DLL_PUBLIC __declspec(dllexport)
#else
#define DLL_PUBLIC __declspec(dllimport)
#endif
#else
#define DLL_PUBLIC __attribute__((visibility("default")))
#endif


#include "IRenderAPI.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanShaderModuleBuilder.hpp"
#include "VulkanRenderPassBuilder.hpp"
#include "VulkanCommandQueueBuilder.hpp"
#include "VulkanBufferBuilder.hpp"
#include "VulkanImageBuilder.hpp"
#include "VulkanGraphicsPipelineBuilder.hpp"
#include "VulkanDescriptorBuilder.hpp"

#include <iostream>

#endif // defined(__linux__) || defined(_WIN32)
namespace Graphics::Vulkan {

	class VulkanDevice;

	class VulkanCommandQueue;

	class DLL_PUBLIC VulkanBackend final : public Backend {
		public:

			explicit VulkanBackend(const char* const* extensions, size_t count);

			~VulkanBackend() override;

			BackendType GetType() override;

			std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* nativeWindowHandleProvider) override;

			std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& surface) override;

			JarShaderModuleBuilder* InitShaderModuleBuilder() override;

			JarRenderPassBuilder* InitRenderPassBuilder() override;

			JarCommandQueueBuilder* InitCommandQueueBuilder() override;

			JarBufferBuilder* InitBufferBuilder() override;

			JarImageBuilder* InitImageBuilder() override;

			JarPipelineBuilder* InitPipelineBuilder() override;

			JarDescriptorBuilder* InitDescriptorBuilder() override;

			// Staging Buffer CommandPool management
			void onRegisterNewBuffer();

			void onDestroyBuffer();

			std::shared_ptr<VulkanCommandQueue> getStagingCommandQueue();

		private:
			std::vector<const char*> extensionNames;
			std::vector<const char*> validationLayers;
			VkInstance instance{};
			VkDebugUtilsMessengerEXT debugMessenger;

			std::shared_ptr<VulkanDevice> m_device;

			// Staging command pool management;
			std::shared_ptr<VulkanCommandQueue> m_stagingCommandQueue;
			uint32_t m_bufferCount;

			void createInstance();

			void enableValidationLayers();

			void createDebugCallbackSender();

			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
			                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			                                                    void* pUserData) {
				std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

				return VK_FALSE;
			}
	};

	extern "C" Graphics::Backend* CreateVulkanBackend(const char* const* extensions, size_t count);


}
#endif //JAREP_VULKANBACKEND_HPP
