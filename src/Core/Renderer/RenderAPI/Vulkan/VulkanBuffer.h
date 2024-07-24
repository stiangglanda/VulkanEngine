#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"

namespace Core 
{
    class VulkanBuffer 
    {
    public:
        VulkanBuffer() {}
        ~VulkanBuffer() {}
        
        void Init(VkInstance instance);
        void Shutdown(VkInstance instance);

    private:
    };
} // namespace Core