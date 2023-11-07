//
// Created by Ryen Zhao on 07/11/2023.
//

#ifndef PIXEL_ENGINE_VULKAN_API_TEST_H
#define PIXEL_ENGINE_VULKAN_API_TEST_H

#pragma once

#include "../library_support/Graphic/vulkan/window.hpp"

namespace graph_vulkan{
    class vulkan_window_test{
    public:
        static constexpr int WIDTH_WINDOW = 1600;
        static constexpr int HEIGHT_WINDOW = 900;

        void run();

    private:
        Window window_test{WIDTH_WINDOW,HEIGHT_WINDOW,"Vulkan Window Test"};
    };
} // namespace graph_vulkan


#endif //PIXEL_ENGINE_VULKAN_API_TEST_H
