cmake_minimum_required(VERSION 3.26)
project(Pixel_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# brew include directory in macOS locally
## glfw
include_directories(/opt/homebrew/include ./include)
## glfw library directory in macOS locally
link_directories(/opt/homebrew/Cellar/glfw/3.3.8/lib)

# VulkanSDK
## Vulkan include directory in macOS locally
include_directories(/Users/ryen/Code/Library/VulkanSDK/1.3.268.1/macOS/include)
## Vulkan library directory in macOS locally
link_directories(/Users/ryen/Code/Library/VulkanSDK/1.3.268.1/macOS/lib)

# compile shader for vulkan
execute_process(
        COMMAND ./shader_compile.sh
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/library_support/Graphic/vulkan/
        RESULT_VARIABLE shader_result
        OUTPUT_VARIABLE shader_output
        ERROR_VARIABLE shader_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
)
message(STATUS "vulkan shader - result: ${shader_result}")
message(STATUS "vulkan shader - output: ${shader_output}")
message(STATUS "vulkan shader - error: ${shader_error}")



set(librariesList
        GLFW
        vulkan
)

add_executable(
        Pixel_Engine
        # main programme
        src/main.cpp
        # library supports
        src/library_support/Graphic/vulkan/window/window.hpp
        src/library_support/Graphic/vulkan/window/window.cpp
        # test parts
        src/test/vulkan_API_test.cpp
        src/test/vulkan_API_test.hpp
        src/library_support/Graphic/vulkan/pipeline/pipeline.hpp
        src/library_support/Graphic/vulkan/pipeline/pipeline.cpp

)

target_link_libraries(Pixel_Engine ${librariesList})