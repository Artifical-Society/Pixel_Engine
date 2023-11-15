#pragma once

#include "../window/window.hpp"

// std lib headers
#include <string>
#include <vector>

namespace graph_vulkan {

    struct Swap_Chain_Support_Details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct Queue_Family_Indices {
        uint32_t graphics_family;
        uint32_t present_family;
        bool graphics_family_has_value = false;
        bool present_family_has_value = false;
        bool is_complete() { return graphics_family_has_value && present_family_has_value; }
    };

    class Device {
    public:
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

        Device(Window &window);
        ~Device();

        // Not copyable or movable
        Device(const Device &) = delete;
        void operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

        VkCommandPool get_command_pool() { return command_pool; }
        VkDevice device() { return device_; }
        VkSurfaceKHR surface() { return surface_; }
        VkQueue graphics_queue() { return graphics_queue_; }
        VkQueue present_queue() { return present_queue_; }

        Swap_Chain_Support_Details get_swap_chain_support() { return query_swap_chain_support(physical_device); }

        uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        Queue_Family_Indices find_physical_queue_families() { return find_queue_families(physical_device); }

        VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &buffer_memory);

        VkCommandBuffer begin_single_time_commands();

        void end_single_time_commands(VkCommandBuffer command_buffer);

        void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

        void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);

        void create_image_with_info(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &image_memory);

        VkPhysicalDeviceProperties properties;

    private:
       void create_instance();
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
       void has_GFLW_required_instance_extensions();
       bool check_device_extension_support(VkPhysicalDevice device);
       Swap_Chain_Support_Details query_swap_chain_support(VkPhysicalDevice device);

       VkInstance instance;
       VkDebugUtilsMessengerEXT debug_messenger;
       VkPhysicalDevice physical_device = VK_NULL_HANDLE;
       Window &window;
       VkCommandPool command_pool;

       VkDevice device_;
       VkSurfaceKHR surface_;
       VkQueue graphics_queue_;
       VkQueue present_queue_;

       const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
       const std::vector<const char *> deviceExtensions = {
               VK_KHR_SWAPCHAIN_EXTENSION_NAME,
       };
    };
}  // namespace graph_vulkan