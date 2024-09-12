#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core
{
class VulkanDebug
{
  public:
    VulkanDebug()
    {
    }
    ~VulkanDebug()
    {
    }

    void Init(VkInstance instance, const bool EnableValidationLayers);
    void Shutdown(VkInstance instance);
    static void logPhysicalDevice(VkPhysicalDevice physicalDevice);

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  private:
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

  private:
    VkDebugUtilsMessengerEXT debugMessenger; // Vulkan debug output handle
    bool mInitialised = false;
};
} // namespace Core
