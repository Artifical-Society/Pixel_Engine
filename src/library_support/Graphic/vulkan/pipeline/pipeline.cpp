//
// Created by Ryen Zhao on 08/11/2023.
//

// match hpp file
#include "pipeline.hpp"
//standard libraries
#include <fstream>


namespace graph_vulkan{
    std::vector<char> Pipeline::read_file(const std::string target_file_path){
        std::ifstream target_file{target_file_path, std::ios::ate | std::ios::binary};
        /**
         *  Explain for std::ios::ate
         *  while file been open, cursor locate the tail of the file to get the size of the file
         *
         *  Explain for std::ios::binary
         *  Read the file by binary mode, to avoid any changes to the file while in reading
         **/

        if (!target_file.is_open()){// check if the file can be open, or throw an error
            throw std::runtime_error("Failed to open file: "+ target_file_path);
        }
        size_t target_file_size = static_cast<size_t>(target_file.tellg());
    }
    void Pipeline::create_graphics_pipeline(const std::string& vert_path, const std::string& frag_path){}
    Pipeline::Pipeline(const std::string& vert_path, const std::string& frag_path){}
\
}
