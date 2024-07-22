#pragma once
#include "../../../vepch.h"
#include <optional>
#include <vulkan/vulkan.h>

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
    VulkanDevice()
    {
    }
    ~VulkanDevice()
    {
    }

    void Init(VkInstance instance, VkSurfaceKHR surface, const bool enableValidationLayers,
              const std::vector<const char *> validationLayers, VkQueue *graphicsQueue, VkQueue *presentQueue);
    void Shutdown();

    VkPhysicalDevice getPhysicalDevice() const
    {
        return physicalDevice;
    }

    VkDevice getDevice() const
    {
        return device;
    }

    static const QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    const QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface) const;

    const uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const; 
    const VkFormat findDepthFormat() const;
    const VkFormat findSupportedFormat(
        const std::vector<VkFormat> &candidates, VkImageTiling tiling,
        VkFormatFeatureFlags features) const;
  private:
    void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    void createLogicalDevice(const bool enableValidationLayers, const std::vector<const char *> validationLayers,
                             VkSurfaceKHR surface, VkQueue *graphicsQueue, VkQueue *presentQueue);
    bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // GPU chosen as the default device
    VkDevice device;                                  // Vulkan device for commands

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
} // namespace Core
