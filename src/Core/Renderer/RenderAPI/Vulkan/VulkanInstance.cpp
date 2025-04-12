#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include <GLFW/glfw3.h>

namespace Core
{

void VulkanInstance::Shutdown()
{
    vkDestroyInstance(instance, nullptr);
    VE_CORE_INFO("Shutdown Vulkan Instance");
}

void VulkanInstance::Init(const bool enableValidationLayers, const std::vector<const char *> validationLayers)
{
    if( volkInitialize() != VK_SUCCESS )
    {
        VE_CORE_ERROR("failed to initialize Volk!");
        throw std::runtime_error("failed to initialize Volk!");
    }
    else
    {
        VE_CORE_INFO("Successful: Init Volk");
    }

    if (enableValidationLayers && !checkValidationLayerSupport(validationLayers))
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions(enableValidationLayers);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        VulkanDebug::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        VE_CORE_ERROR("failed to create instance!");
        throw std::runtime_error("failed to create instance!");
    }
    else
    {
        VE_CORE_INFO("Successful: Init Vulkan Instance");
    }

    volkLoadInstance(instance);
}

std::vector<const char *> VulkanInstance::getRequiredExtensions(const bool enableValidationLayers)
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

#if defined(VE_PLATFORM_WINDOWS) || defined(VE_PLATFORM_LINUX)
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
#else
    return nullptr;
#endif

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanInstance::checkValidationLayerSupport(const std::vector<const char *> validationLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}
} // namespace Core
