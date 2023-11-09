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
    Pipeline::Pipeline(const std::string& vert_path, const std::string& frag_path){
        create_graphics_pipeline(vert_path, frag_path);
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
            const std::string& frag_path
            ){
        auto vert_code = read_file(vert_path);
        auto frag_code = read_file(frag_path);


        // Test if files are readed correctly.
        std::cout << "Vertex Shader Code Size: " << vert_code.size() <<std::endl;
        std::cout << "Fragment Shader Code Size: " << frag_code.size() << std::endl;
    }
}
