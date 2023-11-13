//
// Created by Ryen Zhao on 13/11/2023.
//

#ifndef PIXEL_ENGINE_DEVICE_HPP
#define PIXEL_ENGINE_DEVICE_HPP

#pragma once

#include "../window/window.hpp"

#include <string>
#include <vector>

namespace graph_vulkan{
    struct Swap_Chain_Support_Details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct Queue_Family_Indices {
        uint32_t graphics_Family;
        uint32_t  present_Family;
        bool graphics_Family_has_Value = false;
        bool  present_Family_has_Value = false;
        bool is_complete() {
            return ( graphics_Family_has_Value && present_Family_has_Value );
        }
    };

    class Device{
    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        Window &window;
        VkCommandPool command_pool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphics_queue_;
        VkQueue present_queue_;

        const std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        void create_instance(const char* application_name, std::tuple<int, int, int>application_version);
        void setup_debug_messenger();
        void create_surface();
        void pick_physical_device();
        void create_logical_device();
        void create_command_pool();

        // helper functions
        bool is_device_suitable(VkPhysicalDevice device);
        std::vector<const char *> get_required_extensions();
        bool check_validation_layer_support();
        Queue_Family_Indices find_queue_families(VkPhysicalDevice device);
        void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info);
        void has_GflW_required_instance_extensions();
        bool check_device_extension_support(VkPhysicalDevice device);
        Swap_Chain_Support_Details query_Swap_Chain_Support(VkPhysicalDevice device);

    public:
#ifdef NDEBUG
        const bool enable_validation_layers = false;
#else
        const bool enable_validation_layers = true;
#endif
        Device(
            Window &window,
            std::string application_name,
            std::tuple<int, int, int> application_version
            );
        ~Device();

        Device(const Device &) = delete;
        Device &operator = (const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator = (Device &&) = delete;

        // helper functions
        VkCommandPool get_command_pool(){ return command_pool; }
        VkDevice device(){ return device_; }
        VkSurfaceKHR surface(){ return surface_; }
        VkQueue graphics_queue(){ return graphics_queue_; }
        VkQueue present_queue(){ return present_queue_; }

        Swap_Chain_Support_Details get_Swap_Chain_Support(){ return query_Swap_Chain_Support(physical_device); }
        uint32_t find_Memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

        Queue_Family_Indices find_physical_queue_families(){  return find_queue_families(physical_device); }

        VkFormat find_supported_format(
                const std::vector<VkFormat> &candidates,
                VkImageTiling tiling,
                VkFormatFeatureFlags features
                );

        // buffer helper functions
        void create_buffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &buffer_memory
                );

        VkCommandBuffer begin_single_time_commands();
        void end_single_time_commands(VkCommandBuffer command_buffer);
        void copy_buffer(
                VkBuffer src_buffer,
                VkBuffer dst_buffer,
                VkDeviceSize size
                );
        void copy_buffer_to_image(
                VkBuffer buffer,
                VkImage image,
                uint32_t width,
                uint32_t height,
                uint32_t layerCount
                );
        void create_image_with_info(
                const VkImageCreateInfo &image_info,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &image_memory
                );

        VkPhysicalDeviceProperties properties{};
    };

} // namespace graph_vulkan



#endif //PIXEL_ENGINE_DEVICE_HPP
