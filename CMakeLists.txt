cmake_minimum_required(VERSION 3.26)
project(Pixel_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# brew include directory in macOS locally
## glfw
include_directories(/opt/homebrew/include ./include)
## glfw library directory in macOS locally
link_directories(/opt/homebrew/Cellar/glfw/3.3.8/lib)

## glm
include_directories(/opt/homebrew/include ./include)
## glm library directory in macOS locally
link_directories(/opt/homebrew/Cellar/glm/0.9.9.8/lib)


# VulkanSDK
## Vulkan include directory in macOS locally
include_directories(/Users/ryen/Code/Library/VulkanSDK/1.3.268.1/macOS/include)
## Vulkan library directory in macOS locally
link_directories(/Users/ryen/Code/Library/VulkanSDK/1.3.268.1/macOS/lib)

# compile shader for vulkan

execute_process(
        COMMAND ./compile_shader.sh
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/library_support/Graphic/vulkan
        RESULT_VARIABLE shader_result
        OUTPUT_VARIABLE shader_output
        ERROR_VARIABLE shader_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
)
message(STATUS "vulkan shader - result: ${shader_result}")
message(STATUS "vulkan shader - output: ${shader_output}")
message(STATUS "vulkan shader -  error: ${shader_error}")

execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/src/library_support/Graphic/vulkan/shaders/build
            ${CMAKE_BINARY_DIR}/CMakeFiles/Pixel_Engine.dir/src/library_support/Graphic/vulkan/shaders/build
        RESULT_VARIABLE copy_result
        OUTPUT_VARIABLE copy_output
        ERROR_VARIABLE copy_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
)
message(STATUS "copy shader - result: ${copy_result}")
message(STATUS "copy shader - output: ${copy_output}")
message(STATUS "copy shader -  error: ${copy_error}")

set(librariesList
        vulkan
        GLFW
)

add_executable(
        Pixel_Engine
        # main programme
        src/main.cpp
        # library supports
        src/library_support/Graphic/vulkan/window/window.hpp
        src/library_support/Graphic/vulkan/window/window.cpp

        src/library_support/Graphic/vulkan/pipeline/pipeline.hpp
        src/library_support/Graphic/vulkan/pipeline/pipeline.cpp

        # test parts
        src/test/vulkan_API_test.cpp
        src/test/vulkan_API_test.hpp

        # resources
        ${Shader_Copy}
        src/library_support/Graphic/vulkan/device/device.hpp
        src/library_support/Graphic/vulkan/device/device.cpp

)

target_link_libraries(Pixel_Engine ${librariesList})