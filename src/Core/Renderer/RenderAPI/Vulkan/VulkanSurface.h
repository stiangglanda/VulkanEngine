#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"

namespace Core 
{
    class VulkanSurface 
    {
    public:
        VulkanSurface() {}
        ~VulkanSurface() {}
        
        void Init(VkInstance instance);
        void Shutdown(VkInstance instance);

        VkSurfaceKHR getSurface() const 
        {
            return surface;
        }

    private:
        VkSurfaceKHR surface;   // Vulkan window surface
    };
} // namespace Core