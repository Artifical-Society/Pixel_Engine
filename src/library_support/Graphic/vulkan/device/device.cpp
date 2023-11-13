//
// Created by Ryen Zhao on 13/11/2023.
//

#include "device.hpp"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace graph_vulkan{

    VkResult create_debug_utils_message_EXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugUtilsMessengerEXT *pDebugMessenger
            ){
        auto _function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance,
                "vkCreateDebugUtilsMessengerEXT"
                );

        if (_function != nullptr){
            return _function(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroy_debug_utils_messenger_EXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debug_messenger,
            const VkAllocationCallbacks *pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance,
                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debug_messenger, pAllocator);
        }
    }

    // class member functions
    Device::Device(Window &window, std::string application_name, std::tuple<int, int, int> application_version) : window{window} {
        create_instance(const_cast<char*>(application_name.data()), application_version);
        setup_debug_messenger();
        create_surface();
        pick_physical_device();
        create_logical_device();
        create_command_pool();
    }

    Device::~Device() {
        vkDestroyCommandPool(device_, command_pool, nullptr);
        vkDestroyDevice(device_, nullptr);

        if (enable_validation_layers) {
            destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface_, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    // create functions
    void Device::create_instance(const char* application_name, std::tuple<int, int, int> application_version) {
        if (enable_validation_layers && !check_validation_layer_support()){
            throw std::runtime_error("Validation layers requested, but not available!");
        }
        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = application_name;
        app_info.applicationVersion = VK_MAKE_VERSION(
                std::get<0>(application_version),
                std::get<1>(application_version),
                std::get<2>(application_version)
                );
        app_info.pEngineName = "No Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        auto extensions = get_required_extensions();

        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
        if (enable_validation_layers){
            create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            create_info.ppEnabledExtensionNames = validationLayers.data();

            populate_debug_messenger_create_info(debug_create_info);
            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
        } else {
            create_info.enabledLayerCount = 0;
            create_info.pNext = nullptr;
        }

        if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS){
            std::string error_message = std::string("Failed to create instance: ").append(application_name);
            throw std::runtime_error(error_message);
        }

        has_GflW_required_instance_extensions();
    }

    void Device::pick_physical_device() {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if(device_count == 0){
            throw std::runtime_error("Failed to fin GPUs with Vulkan support.");
        }

        std::cout << "Device count: " << device_count << std::endl;
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

        for(const auto &device : devices){
            if(is_device_suitable(device)){
                physical_device = device;
                break;
            }
        }

        if(physical_device == VK_NULL_HANDLE){
            throw std::runtime_error("Failed to find a suitable GPU!");
        }

        vkGetPhysicalDeviceProperties(physical_device, &properties);

        std::cout << "Physical device: " << properties.deviceName << std::endl;
    }


}