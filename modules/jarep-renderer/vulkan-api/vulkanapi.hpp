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

	class VulkanGraphicsPipeline;

	class VulkanGraphicsPipelineBuilder;

	class VulkanCommandQueueBuilder;

	class VulkanCommandBuffer;

	class VulkanShaderModuleBuilder;

	class VulkanBufferBuilder;

	class VulkanImageBuilder;

	class VulkanImage;

	class VulkanCommandQueue;

	class VulkanDescriptorSet;

#pragma region VulkanBackend{

	class VulkanBackend final : public Backend {
		public:
			explicit VulkanBackend(const std::vector<const char*>& extensions);

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

			[[nodiscard]] uint32_t getMaxSwapchainImageCount() const { return m_swapchainMaxImageCount; }

		private:
			VkExtent2D m_imageExtent;
			VkQueue m_graphicsQueue;
			VkQueue m_presentQueue;
			VkFormat m_swapchainImageFormat;
			VkSwapchainKHR m_swapchain;
			std::vector<VkImage> m_swapchainImages;
			std::vector<VkImageView> m_swapchainImageViews;
			VkImage m_depthImage;
			VkDeviceMemory m_depthImageMemory;
			VkImageView m_depthImageView;
			uint32_t m_currentImageIndex;
			uint32_t m_swapchainMaxImageCount;

			std::vector<std::shared_ptr<VulkanFramebuffer>> m_swapchainFramebuffers;

			std::shared_ptr<VulkanDevice> m_device;

			[[nodiscard]] static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
			                                                 VkExtent2D surfaceExtent);

			static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

			static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

			void createImageViews();

			void createDepthResources();

			VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
			                             VkFormatFeatureFlags features);

			VkFormat findDepthFormat();

			bool hasStencilComponent(VkFormat format);
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

			void BindPipeline(std::shared_ptr<JarPipeline> pipeline, uint32_t frameIndex) override;

			void BindVertexBuffer(std::shared_ptr<JarBuffer> buffer) override;

			void BindIndexBuffer(std::shared_ptr<JarBuffer> indexBuffer) override;

			void Draw() override;

			void DrawIndexed(size_t indexAmount) override;

			void Present(std::shared_ptr<JarSurface>& surface, std::shared_ptr<JarDevice> device) override;

			void Release(VkDevice device);

			static VkCommandBuffer StartSingleTimeRecording(std::shared_ptr<VulkanDevice>& device,
			                                                std::shared_ptr<VulkanCommandQueue>& commandQueue);

			static void EndSingleTimeRecording(
					std::shared_ptr<VulkanDevice>& device,
					VkCommandBuffer commandBuffer,
					std::shared_ptr<VulkanCommandQueue>& commandQueue);

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

			static void
			createBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkDeviceSize size, VkBufferUsageFlags usage,
			             VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

			static uint32_t
			findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		private:
			std::shared_ptr<VulkanBackend> m_backend;
			std::optional<VkBufferUsageFlags> m_bufferUsageFlags;
			std::optional<VkMemoryPropertyFlags> m_memoryPropertiesFlags;
			std::optional<size_t> m_bufferSize;
			std::optional<const void*> m_data;
			static inline uint32_t nextBufferId = 0;

			void copyBuffer(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer srcBuffer, VkBuffer dstBuffer,
			                VkDeviceSize size);

			std::shared_ptr<JarBuffer> BuildUniformBuffer(std::shared_ptr<VulkanDevice> vulkanDevice);

	};


	class VulkanBuffer final : public JarBuffer {
		public:
			VulkanBuffer(std::shared_ptr<VulkanDevice>& device, uint32_t bufferId, VkBuffer buffer, size_t bufferSize,
			             VkDeviceMemory deviceMemory,
			             void* data, const std::function<void()>& bufferReleasedCallback)
					: m_device(device), m_buffer(buffer), m_bufferSize(bufferSize), m_bufferMemory(deviceMemory),
					  m_data(data),
					  m_bufferReleasedCallback(bufferReleasedCallback), id(bufferId) {

			};

			~VulkanBuffer() override;

			void Release() override;

			void Update(const void* data, size_t bufferSize) override;

			[[nodiscard]] VkBuffer getBuffer() const { return m_buffer; }

			[[nodiscard]] size_t getBufferSize() const { return m_bufferSize; }

			uint32_t getBufferId() const { return id; }

		private:
			// Buffer ID management
			uint32_t id;

			// Buffer data
			VkBuffer m_buffer;
			VkDeviceMemory m_bufferMemory;
			size_t m_bufferSize;
			void* m_data;
			std::shared_ptr<VulkanDevice> m_device;

			// Buffer callbacks
			std::function<void()> m_bufferReleasedCallback;
	};

#pragma endregion VulkanBuffer }

#pragma region VulkanDescriptorSet{

	class VulkanDescriptorSetBuilder {
		public:
			VulkanDescriptorSetBuilder(uint32_t swapchainImageCount);

			~VulkanDescriptorSetBuilder() = default;

			std::shared_ptr<VulkanDescriptorSet> Build(std::shared_ptr<VulkanDevice>& vulkanDevice);

			VulkanDescriptorSetBuilder*
			AddUniformBuffers(const std::vector<std::shared_ptr<VulkanBuffer>>& uniformBuffers,
			                  uint32_t binding, VkShaderStageFlags stageFlags);

			VulkanDescriptorSetBuilder* AddImage(const std::shared_ptr<VulkanImage>& image, uint32_t binding,
			                                     VkShaderStageFlags stageFlags);

		private:
			std::vector<VkDescriptorSetLayoutBinding> m_descriptorSetLayoutBindings;
			std::vector<VkDescriptorImageInfo> m_descriptorImageInfos;
			std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes;
			std::unordered_map<uint32_t, std::vector<VkDescriptorBufferInfo>> m_descSetIndexBufferInfosMap;
			std::unordered_map<uint32_t, uint32_t> m_bufferIdToBindingMap;
			std::unordered_map<uint32_t, uint32_t> m_imageIdToBindingMap;

			uint32_t m_maxSwapchainImageCount;
	};


	class VulkanDescriptorSet {
		public:
			VulkanDescriptorSet(std::shared_ptr<VulkanDevice>& device, VkDescriptorPool descriptorPool,
			                    VkDescriptorSetLayout layoutBindings,
			                    std::vector<VkDescriptorSet> descriptorSets) : m_device(device),
			                                                                   m_descriptorPool(descriptorPool),
			                                                                   m_descriptorSetLayout(layoutBindings),
			                                                                   m_descriptorSets(
					                                                                   std::move(descriptorSets)) {}

			~VulkanDescriptorSet() = default;

			void Release();

			[[nodiscard]] VkDescriptorSet const* getDescriptorSetOfFrameIndex(uint32_t frameIndex);

			[[nodiscard]] VkDescriptorSetLayout const* getDescriptorSetLayout() const { return &m_descriptorSetLayout; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkDescriptorSetLayout m_descriptorSetLayout;
			VkDescriptorPool m_descriptorPool;
			std::vector<VkDescriptorSet> m_descriptorSets;
	};

#pragma endregion VulkanDescriptorSet }

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
			                       VkImageView swapchainImageView, VkImageView depthImageView);

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

			VulkanRenderPassBuilder* AddDepthStencilAttachment(DepthAttachment depthStencilAttachment) override;

			std::shared_ptr<JarRenderPass>
			Build(std::shared_ptr<JarDevice> device, std::shared_ptr<JarSurface> surface) override;

		private:
			std::optional<VkAttachmentDescription> m_colorAttachment;
			std::optional<VkAttachmentReference> m_colorAttachmentRef;
			std::optional<VkAttachmentDescription> m_depthStencilAttachment;
			std::optional<VkAttachmentReference> m_depthStencilAttachmentRef;
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
			VulkanGraphicsPipelineBuilder();

			~VulkanGraphicsPipelineBuilder() override;

			VulkanGraphicsPipelineBuilder* SetShaderStage(ShaderStage shaderStage) override;

			VulkanGraphicsPipelineBuilder* SetRenderPass(std::shared_ptr<JarRenderPass> renderPass) override;

			VulkanGraphicsPipelineBuilder* SetVertexInput(VertexInput vertexInput) override;

			VulkanGraphicsPipelineBuilder* SetInputAssemblyTopology(InputAssemblyTopology topology) override;

			VulkanGraphicsPipelineBuilder* SetMultisamplingCount(uint16_t multisamplingCount) override;

			VulkanGraphicsPipelineBuilder*
			BindUniformBuffers(std::vector<std::shared_ptr<JarBuffer>> uniformBuffers, uint32_t binding,
			                   StageFlags stageFlags) override;

			VulkanGraphicsPipelineBuilder*
			BindImageBuffer(std::shared_ptr<JarImage> image, uint32_t binding, StageFlags stageFlags) override;

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
			std::vector<std::shared_ptr<VulkanBuffer>> m_uniformBuffers;
			std::shared_ptr<VulkanRenderPass> m_renderPass;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_pipeline;
			VulkanDescriptorSetBuilder* m_descriptorSetBuilder;

			static VkColorComponentFlags convertToColorComponentFlagBits(ColorWriteMask colorWriteMask);

			static VkShaderStageFlagBits convertToVkShaderStageFlagBits(StageFlags stageFlags);
	};


	class VulkanGraphicsPipeline final : public JarPipeline {
		public:
			VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice>& device, VkPipelineLayout pipelineLayout,
			                       VkPipeline pipeline, std::shared_ptr<VulkanRenderPass>& renderPass,
			                       std::shared_ptr<VulkanDescriptorSet> descriptorSet) :
					m_device(device), m_pipelineLayout(pipelineLayout), m_graphicsPipeline(pipeline),
					m_renderPass(renderPass), m_descriptorSet(std::move(descriptorSet)) {}

			~VulkanGraphicsPipeline() override;

			void Release() override;

			std::shared_ptr<JarRenderPass> GetRenderPass() override;

			[[nodiscard]] VkPipeline getPipeline() const { return m_graphicsPipeline; }

			[[nodiscard]] VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; }

			[[nodiscard]] VkDescriptorSet const* getDescriptorSetFromFrameIndex(size_t frameIndex) const {
				return m_descriptorSet->getDescriptorSetOfFrameIndex(frameIndex);
			}

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_graphicsPipeline;
			std::shared_ptr<VulkanRenderPass> m_renderPass;
			std::shared_ptr<VulkanDescriptorSet> m_descriptorSet;
	};

#pragma endregion VulkanGraphicsPipeline };

#pragma region VulkanImage{

	class VulkanImageBuilder final : public JarImageBuilder {
		public:
			VulkanImageBuilder(std::shared_ptr<VulkanBackend> backend) : m_backend(backend) {};

			~VulkanImageBuilder() override;

			VulkanImageBuilder* SetPixelFormat(PixelFormat format) override;

			VulkanImageBuilder* SetImagePath(std::string imagePath) override;

			std::shared_ptr<JarImage> Build(std::shared_ptr<JarDevice> device) override;

			static void createImage(std::shared_ptr<VulkanDevice>& vulkanDevice, uint32_t texWidth, uint32_t texHeight,
			                        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			                        VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

			static void createImageView(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat format,
			                            VkImageAspectFlags aspectFlags, VkImageView* view);

		private:
			std::optional<PixelFormat> m_pixelFormat;
			std::optional<std::string> m_imagePath;
			static inline uint32_t m_nextImageId = 0;
			std::shared_ptr<VulkanBackend> m_backend;

			void
			transitionImageLayout(std::shared_ptr<VulkanDevice>& vulkanDevice, VkImage image, VkFormat format,
			                      VkImageLayout oldLayout, VkImageLayout newLayout);

			void copyBufferToImage(std::shared_ptr<VulkanDevice>& vulkanDevice, VkBuffer buffer, VkImage image,
			                       uint32_t width, uint32_t height);

			static void createSampler(std::shared_ptr<VulkanDevice>& vulkanDevice, VkSampler& sampler);
	};

	class VulkanImage final : public JarImage {
		public:
			VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage image, VkDeviceMemory imageMemory,
			            VkImageView imageView, VkFormat format, VkExtent2D extent, VkSampler sampler, uint32_t imageId)
					: m_device(device), m_image(image), m_imageMemory(imageMemory), m_imageView(imageView),
					  m_format(format), m_extent(extent), m_sampler(sampler), m_imageId(imageId) {};

			~VulkanImage() override;

			void Release() override;

			[[nodiscard]] VkImage getImage() const { return m_image; }

			[[nodiscard]] VkDeviceMemory getImageMemory() const { return m_imageMemory; }

			[[nodiscard]] VkImageView getImageView() const { return m_imageView; }

			[[nodiscard]] VkFormat getFormat() const { return m_format; }

			[[nodiscard]] VkExtent2D getExtent() const { return m_extent; }

			[[nodiscard]] VkSampler getSampler() const { return m_sampler; }

			[[nodiscard]] uint32_t getImageId() const { return m_imageId; }

		private:
			std::shared_ptr<VulkanDevice> m_device;
			VkImage m_image;
			VkDeviceMemory m_imageMemory;
			VkImageView m_imageView;
			VkFormat m_format;
			VkExtent2D m_extent;
			VkSampler m_sampler;
			uint32_t m_imageId;
	};

#pragma endregion VulkanImage };
}
#endif
#endif //JAREP_VULKANAPI_HPP
