//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_VULKANAPI_HPP
#define JAREP_VULKANAPI_HPP
#if defined(__linux__) or defined(_WIN32)

#include "IRenderer.hpp"
#include <optional>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>
#include "sdlsurfaceadapter.hpp"
#include <iostream>
#include <set>

#if defined (__linux__)
#define VK_USE_PLATFORM_XLIB_KHR

#include <vulkan/vulkan_xlib.h>

#define VK_USE_PLATFORM_WAYLAND_KHR

#include <vulkan/vulkan_wayland.h>

#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif


namespace Graphics::Vulkan {
	class VulkanDevice;

	class VulkanFramebuffer;

	class VulkanSwapchain;

	class VulkanRenderPass;

	class VulkanRenderPassBuilder;

	class VulkanGraphicsPipelineBuilder;

	class VulkanCommandQueueBuilder;

	class VulkanShaderModuleBuilder;

	class VulkanBufferBuilder;

	class VulkanCommandQueue;

#pragma region VulkanBackend{

	class VulkanBackend final : public Backend {
		public:
			explicit VulkanBackend(const std::vector<const char*>& extensions);

			~VulkanBackend() override;

			std::shared_ptr<JarSurface> CreateSurface(NativeWindowHandleProvider* nativeWindowHandleProvider) override;

			std::shared_ptr<JarDevice> CreateDevice(std::shared_ptr<JarSurface>& surface) override;

			JarShaderModuleBuilder* InitShaderModuleBuilder() override;

			JarRenderPassBuilder* InitRenderPassBuilder() override;

			JarCommandQueueBuilder* InitCommandQueueBuilder() override;

			JarBufferBuilder* InitBufferBuilder() override;

			JarPipelineBuilder* InitPipelineBuilder() override;

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

#pragma endregion VulkanBackend }

#pragma region VulkanSurface{

	struct SwapChainSupportDetails;

	class VulkanSurface final : public JarSurface {
		public:
			VulkanSurface(VkSurfaceKHR surface, VkExtent2D surfaceExtend);

			~VulkanSurface() override;

			void Update() override;

			void ReleaseSwapchain() override;

			void FinalizeSurface(std::shared_ptr<VulkanDevice> device);

			SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice) const;

			[[nodiscard]] VkSurfaceKHR getSurface() const { return m_surface; }

			[[nodiscard]] VkExtent2D getSurfaceExtent() const { return m_surfaceExtent; }

			[[nodiscard]] VulkanSwapchain* getSwapchain() const { return m_swapchain.get(); }

		private:
			VkSurfaceKHR m_surface;
			VkExtent2D m_surfaceExtent{};
			std::unique_ptr<VulkanSwapchain> m_swapchain;
	};


#pragma endregion VulkanSurface }

#pragma region VulkanDevice{

	class VulkanDevice final : public JarDevice {
		public:
			VulkanDevice();

			~VulkanDevice() override;

			void Release() override;

			void CreatePhysicalDevice(VkInstance instance, std::shared_ptr<VulkanSurface>& surface);

			void CreateLogicalDevice();

			[[nodiscard]] VkDevice getLogicalDevice() const { return m_device; }

			[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }

			[[nodiscard]] std::optional<uint32_t> getGraphicsFamilyIndex() const { return m_graphicsFamily; }

			[[nodiscard]] std::optional<uint32_t> getPresentFamilyIndex() const { return m_presentFamily; }

		private:
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;
			std::optional<uint32_t> m_graphicsFamily;
			std::optional<uint32_t> m_presentFamily;

			const std::vector<const char*> deviceExtensions = {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			bool isPhysicalDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface);

			void findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, std::shared_ptr<VulkanSurface>& surface);

			bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) const;
	};


#pragma endregion VulkanDevice }

#pragma region VulkanSwapchain{

	class VulkanSwapchain {
		public:
			VulkanSwapchain() = default;

			~VulkanSwapchain() = default;

			void Initialize(const std::shared_ptr<VulkanDevice>& device, VkExtent2D extent,
			                const SwapChainSupportDetails& swapchainSupport, VkSurfaceKHR surface);


			void CreateFramebuffersFromRenderPass(const std::shared_ptr<VulkanRenderPass>& renderPass);

			std::shared_ptr<VulkanFramebuffer> AcquireNewImage(VkSemaphore imageAvailable, VkFence frameInFlight);

			void PresentImage(VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence frameInFlight,
			                  VkCommandBuffer* cmdBuffer);

			void Release();

			[[nodiscard]] VkFormat getSwapchainImageFormat() const { return m_swapchainImageFormat; }

			[[nodiscard]] uint32_t getCurrentImageIndex() const { return m_currentImageIndex; }

		private:
			VkExtent2D m_imageExtent;
			VkQueue m_graphicsQueue;
			VkQueue m_presentQueue;
			VkFormat m_swapchainImageFormat;
			VkSwapchainKHR m_swapchain;
			std::vector<VkImage> m_swapchainImages;
			std::vector<VkImageView> m_swapchainImageViews;
			uint32_t m_currentImageIndex;
			uint32_t m_swapchainMaxImageCount;

			std::vector<std::shared_ptr<VulkanFramebuffer>> m_swapchainFramebuffers;

			std::shared_ptr<VulkanDevice> m_device;

			[[nodiscard]] static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
			                                                 VkExtent2D surfaceExtent);

			static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

			static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

			void createImageViews();
	};

#pragma endregion VulkanSwapchain }

#pragma region VulkanCommandBuffer{

	class VulkanCommandBuffer final : JarCommandBuffer {
		public:
			VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkSemaphore imageAvailableSemaphore,
			                    VkSemaphore renderFinishedSemaphore, VkFence frameInFlightFence);

			~VulkanCommandBuffer() override;

			void
			StartRecording(std::shared_ptr<JarSurface> surface, std::shared_ptr<JarRenderPass> renderPass) override;

			void EndRecording() override;

			void BindPipeline(std::shared_ptr<JarPipeline> pipeline) override;

			void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) override;

			void Draw() override;

			void Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) override;

			void Release(VkDevice device);

		private:
			VkCommandBuffer m_commandBuffer;
			VkSemaphore m_imageAvailableSemaphore;
			VkSemaphore m_renderFinishedSemaphore;
			VkFence m_frameInFlightFence;
	};

#pragma endregion VulkanCommandBuffer }

#pragma  region VulkanCommandQueue{

	class VulkanCommandQueueBuilder final : public JarCommandQueueBuilder {
		public:
			VulkanCommandQueueBuilder() = default;

			~VulkanCommandQueueBuilder() override;

			VulkanCommandQueueBuilder* SetCommandBufferAmount(uint32_t commandBufferAmount) override;

			std::shared_ptr<JarCommandQueue> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<uint32_t> m_amountOfCommandBuffers;
			const uint32_t DEFAULT_COMMAND_BUFFER_COUNT = 3;
	};

	class VulkanCommandQueue final : public JarCommandQueue {
		public:
			VulkanCommandQueue(std::shared_ptr<VulkanDevice>& device, VkCommandPool commandPool,
			                   std::vector<VulkanCommandBuffer*> commandBuffers) : m_device(device),
			                                                                       m_commandPool(commandPool),
			                                                                       m_commandBuffers(
					                                                                       std::move(commandBuffers)) {
				m_currentBufferIndexInUse = 0;
			};

			~VulkanCommandQueue() override;

			JarCommandBuffer* getNextCommandBuffer() override;

			VkCommandPool getCommandPool() { return m_commandPool; }

			void Release() override;

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkCommandPool m_commandPool;
			std::vector<VulkanCommandBuffer*> m_commandBuffers;

			uint32_t m_currentBufferIndexInUse;
	};

#pragma endregion VulkanCommandQueue }

#pragma region VulkanBuffer{

	class VulkanBufferBuilder final : public JarBufferBuilder {

		public:
			VulkanBufferBuilder(std::shared_ptr<VulkanBackend> backend) : m_backend(backend) {}

			~VulkanBufferBuilder() override;

			VulkanBufferBuilder* SetUsageFlags(BufferUsage usageFlags) override;

			VulkanBufferBuilder* SetMemoryProperties(MemoryProperties memProps) override;

			VulkanBufferBuilder* SetBufferData(const void* data, size_t bufferSize) override;

			std::shared_ptr<JarBuffer> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::shared_ptr<VulkanBackend> m_backend;
			std::optional<VkBufferUsageFlags> m_bufferUsageFlags;
			std::optional<VkMemoryPropertyFlags> m_memoryPropertiesFlags;
			std::optional<size_t> m_bufferSize;
			std::optional<const void*> m_data;

			void createBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkDeviceSize size, VkBufferUsageFlags usage,
			                  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

			void copyBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer srcBuffer, VkBuffer dstBuffer,
			                VkDeviceSize size);

			static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
			                               VkMemoryPropertyFlags properties);
	};


	class VulkanBuffer final : public JarBuffer {
		public:
			VulkanBuffer(std::shared_ptr<VulkanDevice>& device, VkBuffer buffer, VkDeviceMemory deviceMemory,
			             const std::function<void()>& bufferReleasedCallback)
					: m_device(device), m_buffer(buffer), m_bufferMemory(deviceMemory),
					  m_bufferReleasedCallback(bufferReleasedCallback) {};

			~VulkanBuffer() override;

			void Release() override;

			[[nodiscard]] VkBuffer getBuffer() const { return m_buffer; }

		private:
			VkBuffer m_buffer;
			VkDeviceMemory m_bufferMemory;
			std::shared_ptr<VulkanDevice> m_device;
			std::function<void()> m_bufferReleasedCallback;
	};

#pragma endregion VulkanBuffer }

#pragma region VulkanShaderModule{

	class VulkanShaderModuleBuilder final : public JarShaderModuleBuilder {
		public:
			VulkanShaderModuleBuilder() = default;

			~VulkanShaderModuleBuilder() override;

			VulkanShaderModuleBuilder* SetShader(std::string shaderCode) override;

			VulkanShaderModuleBuilder* SetShaderType(ShaderType shaderType) override;

			std::shared_ptr<JarShaderModule> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::optional<std::string> m_shaderCodeStr;
			std::optional<ShaderType> m_shaderType;

	};


	class VulkanShaderModule final : public JarShaderModule {
		public:
			VulkanShaderModule(std::shared_ptr<VulkanDevice>& device, VkShaderModule shaderModule,
			                   ShaderType shaderType) : m_device(device), m_shaderModule(shaderModule),
			                                            m_shaderType(shaderType) {};

			~VulkanShaderModule() override;

			void Release() override;

			[[nodiscard]] VkShaderModule getShaderModule() const { return m_shaderModule; }

			[[nodiscard]] ShaderType getShaderType() const { return m_shaderType; }

		private:
			VkShaderModule m_shaderModule;
			ShaderType m_shaderType;
			std::shared_ptr<VulkanDevice> m_device;
	};

#pragma endregion VulkanShaderModule }

#pragma region VulkanFramebuffer{

	class VulkanFramebuffer final : public JarFramebuffer {
		public:
			explicit VulkanFramebuffer(const VkExtent2D framebufferExtent) {
				m_framebuffer = nullptr;
				m_framebufferExtent = framebufferExtent;
			}

			~VulkanFramebuffer() override;

			void CreateFramebuffer(VkDevice device, VkRenderPass renderPass,
			                       VkImageView swapchainImageView);

			[[nodiscard]] VkFramebuffer getFramebuffer() const { return m_framebuffer; }

			[[nodiscard]] VkExtent2D getFramebufferExtent() const { return m_framebufferExtent; }

			void Release(std::shared_ptr<JarDevice> device) override;

		private:
			VkFramebuffer m_framebuffer;
			VkExtent2D m_framebufferExtent{};
	};

#pragma endregion VulkanFrambuffer }

#pragma region VulkanRenderPass{

	class VulkanRenderPassBuilder final : public JarRenderPassBuilder {
		public :
			VulkanRenderPassBuilder() = default;

			~VulkanRenderPassBuilder() override;

			VulkanRenderPassBuilder* AddColorAttachment(ColorAttachment colorAttachment) override;

			std::shared_ptr<JarRenderPass>
			Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) override;

		private:
			std::optional<VkAttachmentDescription> m_colorAttachment;
			std::optional<VkAttachmentReference> m_colorAttachmentRef;


	};

	class VulkanRenderPass final : public JarRenderPass {
		public:
			VulkanRenderPass(std::shared_ptr<VulkanDevice>& device, VkRenderPass renderPass) : m_device(device),
			                                                                                   m_renderPass(
					                                                                                   renderPass) {};

			~VulkanRenderPass() override;

			void Release() override;

			[[nodiscard]] VkRenderPass getRenderPass() const { return m_renderPass; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkRenderPass m_renderPass;
	};

#pragma endregion VulkanRenderPass }

#pragma region VulkanGraphicsPipeline{

	class VulkanGraphicsPipelineBuilder final : public JarPipelineBuilder {
		public:
			VulkanGraphicsPipelineBuilder() = default;

			~VulkanGraphicsPipelineBuilder() override;

			VulkanGraphicsPipelineBuilder* SetShaderStage(ShaderStage shaderStage) override;

			VulkanGraphicsPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			VulkanGraphicsPipelineBuilder* SetVertexInput(VertexInput vertexInput) override;

			VulkanGraphicsPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) override;

			VulkanGraphicsPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) override;

			VulkanGraphicsPipelineBuilder* SetColorBlendAttachments(ColorBlendAttachment colorBlendAttachment) override;

			VulkanGraphicsPipelineBuilder* SetDepthStencilState(DepthStencilState depthStencilState) override;

			std::shared_ptr<JarPipeline> Build(std::shared_ptr<JarDevice> device) override;

		private:
			std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
			std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
			std::optional<VkPipelineVertexInputStateCreateInfo> m_vertexInput;
			std::optional<VkPipelineInputAssemblyStateCreateInfo> m_inputAssembly;
			std::optional<VkPipelineMultisampleStateCreateInfo> m_multisampling;
			std::vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachmentStates;
			std::optional<VkPipelineColorBlendStateCreateInfo> m_colorBlend;
			std::optional<VkPipelineDepthStencilStateCreateInfo> m_depthStencil;
			std::shared_ptr<VulkanRenderPass> m_renderPass;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_pipeline;

			static VkColorComponentFlags convertToColorComponentFlagBits(ColorWriteMask colorWriteMask);
	};


	class VulkanGraphicsPipeline final : public JarPipeline {
		public:
			VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice>& device, VkPipelineLayout pipelineLayout,
			                       VkPipeline pipeline, std::shared_ptr<VulkanRenderPass>& renderPass) : m_device(
					device), m_pipelineLayout(pipelineLayout), m_graphicsPipeline(pipeline), m_renderPass(renderPass) {}

			~VulkanGraphicsPipeline() override;

			void Release() override;

			std::shared_ptr<JarRenderPass> GetRenderPass() override;

			[[nodiscard]] VkPipeline getPipeline() const { return m_graphicsPipeline; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_graphicsPipeline;
			std::shared_ptr<VulkanRenderPass> m_renderPass;
	};

#pragma endregion VulkanGraphicsPipeline };
}

#endif
#endif //JAREP_VULKANAPI_HPP
