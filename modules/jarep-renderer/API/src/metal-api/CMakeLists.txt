cmake_minimum_required(VERSION 3.25)

set(HEADER_FILES MetalBackend.hpp sdlsurfaceadapter.hpp
        MetalCommandQueueBuilder.hpp
        MetalCommandQueue.hpp
        MetalCommandBuffer.hpp
        MetalRenderPassImages.hpp
        MetalRenderPassBuilder.hpp
        MetalDevice.hpp
        MetalSurface.hpp
        MetalBuffer.hpp
        MetalBufferBuilder.hpp
        MetalShaderModuleBuilder.hpp
        MetalShaderLibrary.hpp
        MetalPipelineBuilder.hpp
        MetalPipeline.hpp
        MetalImageBuilder.hpp
        MetalImage.hpp
        MetalDescriptorContent.hpp
        MetalDescriptor.hpp
        MetalDescriptorLayout.hpp
        MetalDescriptorBuilder.hpp
        MetalRenderTargetBuilder.hpp
        MetalRenderTarget.hpp
        MetalDataTypeMaps.hpp)
set(SOURCE_FILES MetalBackend.cpp sdlsurfaceadapter.mm
        MetalCommandQueueBuilder.cpp
        MetalCommandQueue.cpp
        MetalCommandBuffer.cpp
        MetalRenderPassImages.cpp
        MetalRenderPassBuilder.cpp
        MetalDevice.cpp
        MetalSurface.cpp
        MetalImage.cpp
        MetalBuffer.cpp
        MetalBufferBuilder.cpp
        MetalShaderModuleBuilder.cpp
        MetalShaderLibrary.cpp
        MetalRenderPass.cpp
        MetalImageBuilder.cpp
        MetalDescriptorBuilder.cpp
        MetalDescriptorLayout.cpp
        MetalDescriptor.cpp
        MetalDescriptorContent.cpp
        MetalPipelineBuilder.cpp
        MetalPipeline.cpp
        MetalRenderTargetBuilder.cpp
        MetalRenderTarget.cpp)

add_library(METAL_RENDERER_API ${SOURCE_FILES} ${HEADER_FILES})

add_subdirectory(metal)
target_link_libraries(METAL_RENDERER_API PUBLIC METAL_CPP)
target_link_libraries(METAL_RENDERER_API PUBLIC IRendererAPI)

set_target_properties(METAL_RENDERER_API PROPERTIES HEADER_FILES "${HEADER_FILES}")
target_include_directories(METAL_RENDERER_API PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
install(TARGETS METAL_RENDERER_API
        EXPORT METAL_RENDERER_API_Export
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
)


