#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"

namespace Core 
{
    class VulkanImage 
    {
    public:
        VulkanImage() {}
        ~VulkanImage() {}
        
        void Init(VkInstance instance, const bool EnableValidationLayers);
        void Shutdown(VkInstance instance);
    private:
    };
} // namespace Core