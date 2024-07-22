#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"

namespace Core 
{
    class VulkanCommandBuffer 
    {
    public:
        VulkanCommandBuffer() {}
        ~VulkanCommandBuffer() {}
        
        void Init(VkInstance instance, const bool EnableValidationLayers);
        void Shutdown(VkInstance instance);

    private:

    private:

    };
} // namespace Core