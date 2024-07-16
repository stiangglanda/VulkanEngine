#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"
#include <optional>

namespace Core 
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class VulkanDevice 
    {
    public:
        VulkanDevice() {}
        ~VulkanDevice() {}
        
        void Init(VkInstance instance, VkSurfaceKHR surface, const bool enableValidationLayers, const std::vector<const char *> validationLayers, VkQueue* graphicsQueue, VkQueue* presentQueue);
        void Shutdown();

        VkPhysicalDevice getPhysicalDevice() const
        {
            return physicalDevice;
        }

        VkDevice getDevice() const
        {
            return device;
        }

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface);
        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    private:
        void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        void createLogicalDevice(const bool enableValidationLayers, const std::vector<const char *> validationLayers, VkSurfaceKHR surface, VkQueue* graphicsQueue, VkQueue* presentQueue);
        bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        // uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);//not shure

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // GPU chosen as the default device
        VkDevice device;                                  // Vulkan device for commands

        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
} // namespace Core