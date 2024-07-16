#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"

namespace Core 
{
    class VulkanSwapChain 
    {
    public:
        VulkanSwapChain() {}
        ~VulkanSwapChain() {}
        
        void Init(VkInstance instance);
        void Shutdown(VkInstance instance);

    private:
    };
} // namespace Core