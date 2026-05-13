#include "vulkan_utils.h"

#include <iostream>
#include <vector>

#include <libvideo2x/logger_manager.h>

static int enumerate_vulkan_devices(VkInstance* instance, std::vector<VkPhysicalDevice>& devices) {
    // Create a Vulkan instance
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    VkResult result = vkCreateInstance(&create_info, nullptr, instance);
    if (result != VK_SUCCESS) {
        video2x::logger()->error("Nie udało się utworzyć instancji Vulkan.");
        return -1;
    }

    // Wylicz urządzenia fizyczne
    uint32_t device_count = 0;
    result = vkEnumeratePhysicalDevices(*instance, &device_count, nullptr);
    if (result != VK_SUCCESS || device_count == 0) {
        video2x::logger()->error(
            "Nie udało się wyliczyć urządzeń fizycznych Vulkan lub brak dostępnych urządzeń."
        );
        vkDestroyInstance(*instance, nullptr);
        return -1;
    }

    devices.resize(device_count);
    result = vkEnumeratePhysicalDevices(*instance, &device_count, devices.data());
    if (result != VK_SUCCESS) {
        video2x::logger()->error("Nie udało się pobrać urządzeń fizycznych Vulkan.");
        vkDestroyInstance(*instance, nullptr);
        return -1;
    }

    return 0;
}

int list_vulkan_devices() {
    VkInstance instance;
    std::vector<VkPhysicalDevice> physical_devices;
    int result = enumerate_vulkan_devices(&instance, physical_devices);
    if (result != 0) {
        return result;
    }

    uint32_t device_count = static_cast<uint32_t>(physical_devices.size());

    // List Vulkan device information
    for (uint32_t i = 0; i < device_count; i++) {
        VkPhysicalDevice device = physical_devices[i];
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);

        // Wydrukuj ID i nazwę urządzenia Vulkan
        std::cout << i << ". " << device_properties.deviceName << std::endl;
        std::cout << "\tTyp: ";
        switch (device_properties.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                std::cout << "Zintegrowany GPU";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                std::cout << "Dedykowany GPU";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                std::cout << "Wirtualny GPU";
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                std::cout << "CPU";
                break;
            default:
                std::cout << "Nieznany";
                break;
        }
        std::cout << std::endl;

        // Wydrukuj wersję API Vulkan
        std::cout << "\tWersja API Vulkan: " << VK_VERSION_MAJOR(device_properties.apiVersion)
                  << "." << VK_VERSION_MINOR(device_properties.apiVersion) << "."
                  << VK_VERSION_PATCH(device_properties.apiVersion) << std::endl;

        // Wydrukuj wersję sterownika
        std::cout << "\tWersja sterownika: " << VK_VERSION_MAJOR(device_properties.driverVersion)
                  << "." << VK_VERSION_MINOR(device_properties.driverVersion) << "."
                  << VK_VERSION_PATCH(device_properties.driverVersion) << std::endl;

        // Wydrukuj ID urządzenia
        std::cout << "\tID urządzenia: " << std::hex << std::showbase << device_properties.deviceID
                  << std::dec << std::endl;
    }

    // Clean up Vulkan instance
    vkDestroyInstance(instance, nullptr);
    return 0;
}

int get_vulkan_device_prop(uint32_t vk_device_index, VkPhysicalDeviceProperties* dev_props) {
    if (dev_props == nullptr) {
        video2x::logger()->error("Nieprawidłowy wskaźnik właściwości urządzenia.");
        return -1;
    }

    VkInstance instance;
    std::vector<VkPhysicalDevice> devices;
    int result = enumerate_vulkan_devices(&instance, devices);
    if (result != 0) {
        return result;
    }

    uint32_t device_count = static_cast<uint32_t>(devices.size());

    // Check if the Vulkan device ID is valid
    if (vk_device_index >= device_count) {
        vkDestroyInstance(instance, nullptr);
        return -2;
    }

    // Get device properties for the specified Vulkan device ID
    vkGetPhysicalDeviceProperties(devices[vk_device_index], dev_props);

    // Clean up Vulkan instance
    vkDestroyInstance(instance, nullptr);

    return 0;
}
