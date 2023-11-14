/**
 * library_support/Graphic/vulkan/pipeline
 *
 * An API package for window usage based on vulkan and GLFW
 * Author: Ryen (Ruihan Zhao)
 * Create Date: 2023-11-06 14:37:20 UTC +8:00
 *
 **/
#ifndef PIXEL_ENGINE_GRAPHIC_VULKAN_PIPELINE_HPP
#define PIXEL_ENGINE_GRAPHIC_VULKAN_PIPELINE_HPP

#pragma once

#include <string>
#include <vector>

#include "../device/device.hpp"

namespace graph_vulkan{
    struct Pipeline_config_info{};

    class Pipeline {
        private:
            Device& pipeline_device;
            VkPipeline graphics_pipeline;
            VkShaderModule vert_shader_module;
            VkShaderModule frag_shader_module;

            static std::vector<char> read_file(const std::string& target_file_path);

            void create_graphics_pipeline(
                    const std::string& vert_path,
                    const std::string& frag_path,
                    const Pipeline_config_info& config_info);

            void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);


        public:
            Pipeline(
                    Device& device,
                    const std::string& vert_path,
                    const std::string& frag_path,
                    const Pipeline_config_info& config_info);
            ~Pipeline();

            Pipeline(const Pipeline&) = delete;
            void operator = (const Pipeline&) = delete;

            static Pipeline_config_info default_Pipeline_config_info(uint32_t width, uint32_t height);
    };
}


#endif // PIXEL_ENGINE_GRAPHIC_VULKAN_PIPELINE_HPP
