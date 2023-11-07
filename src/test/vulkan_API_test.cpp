
#include "vulkan_API_test.hpp"


namespace graph_vulkan{
    void vulkan_window_test::run() {
        while (!window_test.shouldClose()){
             glfwPollEvents();
        }
    }
}