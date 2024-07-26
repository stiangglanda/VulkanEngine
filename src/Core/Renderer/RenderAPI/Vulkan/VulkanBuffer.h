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
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    private:
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
    };
} // namespace Core