/**
 * library_support/Graphic/vulkan/pipeline
 *
 * An API package for window usage based on vulkan and GLFW
 * Author: Ryen (Ruihan Zhao)
 * Create Date: 2023-11-06 14:37:20 UTC +8:00
 *
 **/
#ifndef PIXEL_ENGINE_GRAPHIC_VULKAN_PIPELINE_H
#define PIXEL_ENGINE_GRAPHIC_VULKAN_PIPELINE_H

#pragma once

#include <string>
#include <vector>

namespace graph_vulkan{
    class Pipeline {
        private:
            static std::vector<char> read_file(const std::string& target_file_path);
            void create_graphics_pipeline(const std::string& vert_path, const std::string& frag_path);
        public:
            Pipeline(const std::string& vert_path, const std::string& frag_path);
    };

}


#endif // PIXEL_ENGINE_GRAPHIC_VULKAN_PIPELINE_H
