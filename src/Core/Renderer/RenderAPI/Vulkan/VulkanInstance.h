#pragma once
#include <volk.h>
#include "../../../vepch.h"

namespace Core
{

class VulkanInstance
{
  public:
    VulkanInstance()
    {
    }
    ~VulkanInstance()
    {
    }

    void Init(const bool enableValidationLayers, const std::vector<const char *> validationLayers);

    void Shutdown();

    VkInstance getInstance() const
    {
        return instance;
    }

  private:
    bool checkValidationLayerSupport(const std::vector<const char *> validationLayers);
    std::vector<const char *> getRequiredExtensions(const bool enableValidationLayers);

  private:
    VkInstance instance; // Vulkan library handle
};
} // namespace Core
