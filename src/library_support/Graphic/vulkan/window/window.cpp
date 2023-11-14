
#include "window.hpp"

namespace graph_vulkan{

    Window::Window(
            int input_width, int input_height,
            std::string input_window_name
            ) : width(input_width), height(input_height), name(input_window_name) {
        initWindow();
    }

    Window::~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow() {
        glfwInit(); // initialize glfw library
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

        window = glfwCreateWindow(
                width, height,
                name.c_str(),
                nullptr, nullptr
                );
    }

    bool Window::should_close() {
        return glfwWindowShouldClose(window);
    }

    void Window::create_window_surface(VkInstance instance, VkSurfaceKHR *surface){
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("Failed to create window surface.");
        }
    }

}
