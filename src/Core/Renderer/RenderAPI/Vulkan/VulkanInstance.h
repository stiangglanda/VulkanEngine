#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core 
{

    class VulkanInstance 
    {
    public:
        VulkanInstance() {}
        ~VulkanInstance() {}

        void Init(const bool enableValidationLayers, const std::vector<const char *> validationLayers);

        void Shutdown();

        VkInstance getInstance() 
        {
            return instance;
        }

    private:
        bool checkValidationLayerSupport(const std::vector<const char *> validationLayers);
        std::vector<const char *> getRequiredExtensions(const bool enableValidationLayers);
        // void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        // void setupDebugMessenger(const bool enableValidationLayers);
        // VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        //                               const VkAllocationCallbacks *pAllocator,
        //                               VkDebugUtilsMessengerEXT *pDebugMessenger);
        // static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        //                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
        //                                                 const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        //                                                 void *pUserData)
        // {
        //     VE_CORE_INFO("validation layer: {0}", pCallbackData->pMessage); // TODO should be split up into info warn and error
        //     return VK_FALSE;
        // }

    private:
        VkInstance instance;// Vulkan library handle

        // VkDebugUtilsMessengerEXT debugMessenger; // Vulkan debug output handle
	};
}