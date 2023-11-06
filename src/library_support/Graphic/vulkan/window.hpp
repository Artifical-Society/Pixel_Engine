/**
 * An API package for window usage based on vulkan and GLFW
 * Author: Ryen (Ruihan Zhao)
 * Create Date: 2023-11-06 14:37:20 UTC +8:00
 **/

#pragma once

#include <string>
#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_VULKAN

namespace graph_vulkan {
    class Window {
    private:
        GLFWwindow *window;
        // measures of window
        std::string name;
        const int width;
        const int height;

        void initWindow();

    public:
        Window(int input_width, int input_height, std::string input_window_name);

        ~Window();
    };
}