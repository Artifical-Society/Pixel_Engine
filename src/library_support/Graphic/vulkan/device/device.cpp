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
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallback_Data,
            void *pUser_Data
            ){
        std::cerr << "Validation layer: " << pCallback_Data->pMessage << std::endl;

        return VK_FALSE;
    }

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
    Device::Device(
            Window &window,
            std::string application_name,
            std::tuple<int, int, int> application_version
            ) : window{window} {

        create_instance(
                const_cast<char*>(application_name.data()),
                application_version
                );
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
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledExtensionNames = validation_layers.data();

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

    void Device::create_logical_device() {
        Queue_Family_Indices indices = find_queue_families(physical_device);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {
                indices.graphics_Family,
                indices.present_Family
        };

        float queue_priority = 1.0f;
        for(uint32_t queue_family : unique_queue_families){
            VkDeviceQueueCreateInfo queue_create_info = {};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features = {};
        device_features.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();

        create_info.pEnabledFeatures = &device_features;
        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        if(enable_validation_layers){
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledExtensionNames = validation_layers.data();
        } else {
            create_info.enabledLayerCount = 0;
        }

        if(vkCreateDevice(physical_device, &create_info, nullptr, &device_) != VK_SUCCESS){
            throw std::runtime_error("Failed to create logical device.");
        }

        vkGetDeviceQueue(device_, indices.graphics_Family, 0, &graphics_queue_);
        vkGetDeviceQueue(device_, indices.present_Family,  0, &present_queue_);
    }

    void Device::create_command_pool() {
        Queue_Family_Indices queue_family_indices = find_physical_queue_families();

        VkCommandPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_Family;
        pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool) != VK_SUCCESS){
            throw std::runtime_error("Failed to create command pool. ");
        }
    }

    void Device::create_surface() {
        window.create_window_surface(instance, &surface_);
    }

    bool Device::is_device_suitable(VkPhysicalDevice device) {
        Queue_Family_Indices indices = find_queue_families(device);

        bool extensions_supported = check_device_extension_support(device);

        bool swap_chain_adequate = false;
        if(extensions_supported){
            Swap_Chain_Support_Details swap_chain_support = query_Swap_Chain_Support(device);

            swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.presentModes.empty();
        }
        VkPhysicalDeviceFeatures supported_features;
        vkGetPhysicalDeviceFeatures(device, &supported_features);

        return indices.is_complete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
    }

    void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info) {
        create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debug_callback;
        create_info.pUserData = nullptr;
    }

    void Device::setup_debug_messenger() {
        if(!enable_validation_layers) return;
        VkDebugUtilsMessengerCreateInfoEXT create_info;
        populate_debug_messenger_create_info(create_info);
        if(create_debug_utils_message_EXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS){
            throw std::runtime_error("Failed to set up debug messenger.");
        }
    }

    bool Device::check_validation_layer_support() {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for(const char *layer_name : validation_layers){
            bool layer_found = false;

            for (const auto &layer_properities : available_layers){
                if(strcmp(layer_name, layer_properities.layerName) == 0){
                    layer_found = true;
                    break;
                }
            }

            if(!layer_found) return false;
        }
        return true;
    }

    std::vector<const char *> Device::get_required_extensions() {
        uint32_t  glfw_Extension_count = 0;
        const char **glfw_Extensions;
        glfw_Extensions = glfwGetRequiredInstanceExtensions(&glfw_Extension_count);

        std::vector<const char *> extensions(glfw_Extensions, glfw_Extensions + glfw_Extension_count);

        if (enable_validation_layers){
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    void Device::has_GflW_required_instance_extensions() {
        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

        std::cout << "Available extensions: " << std::endl;
        std::unordered_set<std::string> available;
        for (const auto &extension : extensions){
            std::cout << "\t" << extension.extensionName << std::endl;
            available.insert(extension.extensionName);
        }
    }

    bool Device::check_device_extension_support(VkPhysicalDevice device) {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(
                device,
                nullptr,
                &extension_count,
                available_extensions.data()
                );

        std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

        for(const auto &extension : available_extensions){
            required_extensions.erase(extension.extensionName);
        }

        return required_extensions.empty();
    }

    Queue_Family_Indices Device::find_queue_families(VkPhysicalDevice device) {
        Queue_Family_Indices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
                device,
                &queue_family_count,
                nullptr
                );

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        int _i = 0;
        for(const auto &queue_family : queue_families){
            if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT){
                indices.graphics_Family = _i;
                indices.graphics_Family_has_Value = true;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, _i, surface_, &presentSupport);
            if (queue_family.queueCount > 0 && presentSupport){
                indices.present_Family = _i;
                indices.present_Family_has_Value = true;
            }
            if (indices.is_complete()){
                break;
            }
            _i++;
        }
        return indices;
    }

    Swap_Chain_Support_Details Device::query_Swap_Chain_Support(VkPhysicalDevice device) {
        Swap_Chain_Support_Details details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                device,
                surface_,
                &details.capabilities
                );

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
                device,
                surface_,
                &format_count,
                nullptr
                );

        if (format_count != 0) {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                    device,
                    surface_,
                    &format_count,
                    details.formats.data()
                    );
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);

        if(present_mode_count != 0){
            details.presentModes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                    device,
                    surface_,
                    &present_mode_count,
                    details.presentModes.data()
                    );
        }
        return details;
    }

    VkFormat Device::find_supported_format(
            const std::vector<VkFormat> &candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features){
                return format;
            } else if (
                    tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            } else {
                continue;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    uint32_t Device::find_Memory_type(uint32_t type_filter, VkMemoryPropertyFlags property_flags) {
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
            if ((type_filter & (1 << i)) &&
                (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void Device::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags property_flags,
                               VkBuffer &buffer, VkDeviceMemory &buffer_memory) {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create vertex buffer!");
            }

            VkMemoryRequirements memory_requirements;
            vkGetBufferMemoryRequirements(device_, buffer, &memory_requirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memory_requirements.size;
            allocInfo.memoryTypeIndex = find_Memory_type(memory_requirements.memoryTypeBits, property_flags);

            if (vkAllocateMemory(device_, &allocInfo, nullptr, &buffer_memory) != VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate vertex buffer memory!");
            }

            vkBindBufferMemory(device_, buffer, buffer_memory, 0);
    }

    VkCommandBuffer Device::begin_single_time_commands(){
        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandPool = command_pool;
        allocate_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device_, &allocate_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        return command_buffer;
    }

    void Device::end_single_time_commands(VkCommandBuffer command_buffer){
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue_);

        vkFreeCommandBuffers(device_, command_pool, 1, &command_buffer);
    }

    void Device::copy_buffer(
            VkBuffer src_buffer,
            VkBuffer dst_buffer,
            VkDeviceSize size ){
        VkCommandBuffer command_buffer = begin_single_time_commands();

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        end_single_time_commands(command_buffer);
    }

    void Device::copy_buffer_to_image(
            VkBuffer buffer,
            VkImage image,
            uint32_t width,
            uint32_t height,
            uint32_t layer_count ){
        VkCommandBuffer command_buffer = begin_single_time_commands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layer_count;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(
                command_buffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);
        end_single_time_commands(command_buffer);
    }

    void Device::create_image_with_info(
            const VkImageCreateInfo &image_info,
            VkMemoryPropertyFlags property_flags,
            VkImage &image,
            VkDeviceMemory &image_memory ){
        if (vkCreateImage(device_, &image_info, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(device_, image, &memory_requirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memory_requirements.size;
        allocInfo.memoryTypeIndex = find_Memory_type(memory_requirements.memoryTypeBits, property_flags);

        if (vkAllocateMemory(device_, &allocInfo, nullptr, &image_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        if (vkBindImageMemory(device_, image, image_memory, 0) != VK_SUCCESS) {
            throw std::runtime_error("failed to bind image memory!");
        }
    }
} // namespace graph_vulkan
