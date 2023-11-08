
#ifndef PIXEL_ENGINE_PIPELINE_H
#define PIXEL_ENGINE_PIPELINE_H

#pragma once

#include <string>
#include <vector>

namespace graph_vulkan{
    class Pipeline {
        private:
            static std::vector<char> read_file(const std::string target_file_path);
            void create_graphics_pipeline(const std::string& vert_path, const std::string& frag_path);
        public:
            Pipeline(const std::string& vert_path, const std::string& frag_path);
    };

}


#endif //PIXEL_ENGINE_PIPELINE_H
