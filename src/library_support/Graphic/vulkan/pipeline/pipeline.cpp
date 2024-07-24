/**
 * library_support/Graphic/vulkan/window
 *
 **/

// match hpp file
#include "pipeline.hpp"
//standard libraries
#include <fstream>
#include <stdexcept>
#include <iostream>

#include <filesystem>

namespace graph_vulkan{
    Pipeline::Pipeline(
            Device& device,
            const std::string& vert_path,
            const std::string& frag_path,
            const Pipeline_config_info& config_info) : pipeline_device(device) {
        create_graphics_pipeline(vert_path, frag_path, config_info);
    }
    Pipeline::~Pipeline() {
        vkDestroyShaderModule(pipeline_device.device(), vert_shader_module, nullptr);
        vkDestroyShaderModule(pipeline_device.device(), frag_shader_module, nullptr);
        vkDestroyPipeline(pipeline_device.device(), graphics_pipeline, nullptr);
    }

    std::vector<char> Pipeline::read_file(const std::string& target_file_path){
        std::ifstream target_file{
            target_file_path,
            std::ios::ate | std::ios::binary
                /**
                 *  Explain for std::ios::ate
                 *  while file been open, cursor locate the tail of the file to get the size of the file
                 *
                 *  Explain for std::ios::binary
                 *  Read the file by binary mode, to avoid any changes to the file while in reading
                 **/
        };

        // check if the file can be open, or throw an error
        if (!target_file.is_open())
            throw std::runtime_error(
                    "Failed to open file: " + target_file_path +
                    "\nCurrent Path: " + (std::filesystem::current_path()).string());


        // get the size of target file
        size_t target_file_size = static_cast<size_t>(target_file.tellg());
        // Create a cache for the file
        std::vector<char> buffer(target_file_size);

        target_file.seekg(0);   // move the cursor to the start of the file
        target_file.read(buffer.data(), target_file_size);
        target_file.close();    //close the file

        return buffer;
    }
    void Pipeline::create_graphics_pipeline(
            const std::string& vert_path,
            const std::string& frag_path,
            const Pipeline_config_info& config_info
            ){
        auto vert_code = read_file(vert_path);
        auto frag_code = read_file(frag_path);


        // Test if files are readed correctly.
        std::cout << "Vertex Shader Code Size: " << vert_code.size() <<std::endl;
        std::cout << "Fragment Shader Code Size: " << frag_code.size() << std::endl;
    }

    void Pipeline::create_shader_module(const std::vector<char> &code, VkShaderModule *shader_module) {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(pipeline_device.device(), &create_info, nullptr,shader_module) != VK_SUCCESS){
            throw std::runtime_error("Failed to create shader module.");
        }
    }

    Pipeline_config_info Pipeline::default_Pipeline_config_info(uint32_t width, uint32_t height) {
        Pipeline_config_info config_info{};

        config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

        config_info.viewport.x = 0.0f;
        config_info.viewport.y = 0.0f;
        config_info.viewport.x = 0.0f;
        config_info.viewport.width = static_cast<float>(width);
        config_info.viewport.height = static_cast<float>(height);
        config_info.viewport.minDepth = 0.0f;
        config_info.viewport.maxDepth = 1.0f;

        config_info.scissor.offset = {0, 0};
        config_info.scissor.extent = {width, height};

        config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config_info.viewport_info.viewportCount = 1;
        config_info.viewport_info.pViewports = &config_info.viewport;
        config_info.viewport_info.scissorCount = 1;
        config_info.viewport_info.pScissors = &config_info.scissor;

        config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config_info.rasterization_info.depthClampEnable = VK_FALSE;
        config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
        config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
        config_info.rasterization_info.lineWidth = 1.0f;
        config_info.rasterization_info.cullMode = VK_CULL_MODE_NONE;
        config_info.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        config_info.rasterization_info.depthBiasEnable = VK_FALSE;
        config_info.rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
        config_info.rasterization_info.depthBiasClamp = 0.0f;           // Optional
        config_info.rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

        config_info.multi_sample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config_info.multi_sample_info.sampleShadingEnable = VK_FALSE;
        config_info.multi_sample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config_info.multi_sample_info.minSampleShading = 1.0f;           // Optional
        config_info.multi_sample_info.pSampleMask = nullptr;             // Optional
        config_info.multi_sample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
        config_info.multi_sample_info.alphaToOneEnable = VK_FALSE;       // Optional

        config_info.color_blend_attachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        config_info.color_blend_attachment.blendEnable = VK_FALSE;
        config_info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config_info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config_info.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        config_info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config_info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config_info.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        config_info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config_info.color_blend_info.logicOpEnable = VK_FALSE;
        config_info.color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
        config_info.color_blend_info.attachmentCount = 1;
        config_info.color_blend_info.pAttachments = &config_info.color_blend_attachment;
        config_info.color_blend_info.blendConstants[0] = 0.0f;  // Optional
        config_info.color_blend_info.blendConstants[1] = 0.0f;  // Optional
        config_info.color_blend_info.blendConstants[2] = 0.0f;  // Optional
        config_info.color_blend_info.blendConstants[3] = 0.0f;  // Optional

        config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
        config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
        config_info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
        config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        config_info.depth_stencil_info.minDepthBounds = 0.0f;  // Optional
        config_info.depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
        config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;
        config_info.depth_stencil_info.front = {};  // Optional
        config_info.depth_stencil_info.back = {};   // Optional
        
        return config_info;
    }


}
