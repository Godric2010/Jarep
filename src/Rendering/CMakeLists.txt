cmake_minimum_required(VERSION 3.16)

project(Rendering LANGUAGES CXX)

set(LIBRARY_NAME Rendering)
set(LIBRARY_VERSION 0.1.0)

include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)
find_package(Vulkan REQUIRED)

add_library(${LIBRARY_NAME})


target_sources(${LIBRARY_NAME} PRIVATE
        include/${LIBRARY_NAME}/IRenderer.hpp
        include/${LIBRARY_NAME}/RendererCreator.hpp
        include/${LIBRARY_NAME}/Core/RenderObject.hpp
        include/${LIBRARY_NAME}/Core/CameraObject.hpp
        src/RenderManager.cpp
        src/core/VulkanCore.cpp
        src/core/VulkanInstance.cpp
        src/core/VulkanSurface.cpp
        src/core/VulkanDevice.cpp
        src/core/VulkanCommandPool.cpp
        src/core/VulkanBuffer.cpp
        src/core/VulkanUniformBuffer.hpp
        src/core/VulkanImage.cpp
        src/core/VulkanImageOperations.cpp
        src/core/VulkanSampler.cpp
        src/core/VulkanSamplerPresets.cpp
        src/core/VulkanDescriptorSetLayout.cpp
        src/core/VulkanDescriptorPool.cpp
        src/core/VulkanDescriptorSet.cpp
        src/core/VulkanSemaphore.cpp
        src/core/VulkanFence.cpp
        src/pipeline/VulkanSwapchain.cpp
        src/pipeline/VulkanRenderPass.cpp
        src/pipeline/VulkanFramebuffer.cpp
        src/pipeline/VulkanPipeline.cpp
        src/pipeline/VulkanOffscreenTarget.cpp
        src/pipeline/VulkanDepthBuffer.cpp
        src/steps/RenderStepManager.cpp
        src/steps/MainPassStep.cpp
        src/steps/FinalBlitStep.cpp
        src/meshes/VulkanMesh.cpp
        src/meshes/VulkanMeshRegistry.cpp
        src/VulkanVertexLayout.cpp
)

if (WIN32)
    target_sources(${LIBRARY_NAME} PRIVATE
            src/core/VulkanSurface_Windows.cpp
    )
elseif (UNIX)
    target_sources(${LIBRARY_NAME} PRIVATE src/core/VulkanSurface_LinuxX11.cpp)
endif ()


target_include_directories(${LIBRARY_NAME} PUBLIC include ${VULKAN_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME}
        PUBLIC EngineCore
        PRIVATE Vulkan::Vulkan
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

generate_doxygen_with_markdown(${LIBRARY_NAME})
add_library_tests(${LIBRARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests)
