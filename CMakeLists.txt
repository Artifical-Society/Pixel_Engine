cmake_minimum_required(VERSION 3.26)
project(Pixel_Engine)

set(CMAKE_CXX_STANDARD 17)

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

set(librariesList
        GLFW
        vulkan
)

add_executable(
        Pixel_Engine src/main.cpp
        src/library_support/Graphic/vulkan/window.hpp
        src/library_support/Graphic/vulkan/window.cpp
        #        src/utility/Non_destructive_data/decimal.cpp
        #        src/utility/Non_destructive_data/decimal.h
        #        src/utility/Non_destructive_data/integer.cpp
        #        src/utility/Non_destructive_data/integer.h

)

target_link_libraries(Pixel_Engine ${librariesList})
