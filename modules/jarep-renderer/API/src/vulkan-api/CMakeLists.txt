cmake_minimum_required(VERSION 3.25)

set(HEADER_FILES sdlsurfaceadapter.hpp
        VulkanBackend.hpp
        VulkanGraphicsPipeline.hpp
        VulkanGraphicsPipelineBuilder.hpp
        VulkanImage.hpp
        VulkanBuffer.hpp
        VulkanBufferBuilder.hpp
        VulkanSurface.hpp
        VulkanDevice.hpp
        VulkanSwapchain.hpp
        VulkanCommandBuffer.hpp
        VulkanCommandQueue.hpp
        VulkanCommandQueueBuilder.hpp
        VulkanDescriptor.hpp
        VulkanDescriptorLayout.hpp
        VulkanDescriptorBuilder.hpp
        VulkanShaderModule.hpp
        VulkanShaderModuleBuilder.hpp
        VulkanFramebuffer.hpp
        VulkanRenderPass.hpp
        VulkanRenderPassFramebuffers.hpp
        VulkanRenderPassBuilder.hpp
        VulkanDataTypeMaps.hpp
        VulkanRenderTargetBuilder.hpp
        VulkanRenderTarget.hpp
        VulkanImageBuffer.hpp
        VulkanImageBufferBuilder.hpp
        VulkanFramebufferBuilder.hpp
)
set(SOURCE_FILES VulkanBackend.cpp sdlsurfaceadapter.cpp
        VulkanGraphicsPipeline.cpp
        VulkanGraphicsPipelineBuilder.cpp
        VulkanImage.cpp
        VulkanImageBuilder.cpp
        VulkanBuffer.cpp
        VulkanBufferBuilder.cpp
        VulkanSurface.cpp
        VulkanDevice.cpp
        VulkanSwapchain.cpp
        VulkanCommandBuffer.cpp
        VulkanCommandQueue.cpp
        VulkanCommandQueueBuilder.cpp
        VulkanDescriptor.cpp
        VulkanDescriptorLayout.cpp
        VulkanDescriptorBuilder.cpp
        VulkanShaderModule.cpp
        VulkanShaderModuleBuilder.cpp
        VulkanFramebuffer.cpp
        VulkanRenderPass.cpp
        VulkanRenderPassFramebuffers.cpp
        VulkanRenderPassBuilder.cpp
        VulkanRenderTargetBuilder.cpp
        VulkanImageBuffer.cpp
        VulkanImageBufferBuilder.cpp
        VulkanFramebufferBuilder.cpp
)

add_library(VULKAN_RENDERER_API STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(VULKAN_RENDERER_API PRIVATE BUILDING_DLL)

find_package(Vulkan REQUIRED)
target_link_libraries(VULKAN_RENDERER_API PUBLIC Vulkan::Vulkan IRendererAPI)

target_include_directories(VULKAN_RENDERER_API PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

install(TARGETS VULKAN_RENDERER_API
        EXPORT VULKAN_RENDERER_API_Export
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
)
