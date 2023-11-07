/**
 *
 *
 *
 **/


#include "./test/vulkan_API_test.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main(){
    graph_vulkan::vulkan_window_test test_instance{};

    try{
        test_instance.run();
    }catch(const std::exception &Exception){
        std::cerr << Exception.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}