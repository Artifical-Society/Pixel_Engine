/**
 * An API package for window usage based on vulkan and GLFW
 * Author: Ryen (Ruihan Zhao)
 * Create Date: 2023-11-06 14:37:20 UTC +8:00
 **/
#include "window.hpp"

namespace graph_vulkan{
    Window::Window(int input_width, int input_height, std::string input_window_name) : width(input_width), height(input_height), name(input_window_name) {
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

        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    }
}
