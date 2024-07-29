#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"
#include <vk_mem_alloc.h>
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

namespace Core 
{
    class VulkanBuffer 
    {
    public:
        VulkanBuffer() {}
        ~VulkanBuffer() {}
        
        void Init(VkInstance instance);
        void Shutdown(VkInstance instance);

        VulkanBuffer(const VulkanBuffer &) = delete;

	    VulkanBuffer &operator=(const VulkanBuffer &) = delete;

	    VulkanBuffer &operator=(VulkanBuffer &&) = delete;

        static void createBuffer(const VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        static void copyBuffer(std::weak_ptr<VulkanCommandBuffer> command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    private:
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
    };
} // namespace Core