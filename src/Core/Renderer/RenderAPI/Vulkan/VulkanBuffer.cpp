#include "VulkanBuffer.h"
#include <GLFW/glfw3.h>
#include "../../../Application.h"

namespace Core 
{

void VulkanBuffer::Init(VkInstance instance)
{
    VE_CORE_INFO("Successful: Init VulkanBuffer");
}

void VulkanBuffer::Shutdown(VkInstance instance)
{
    VE_CORE_INFO("Shutdown VulkanBuffer");
}

void VulkanBuffer::createBuffer(const VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                               VkBuffer &buffer, VkDeviceMemory &bufferMemory) // TODO should be in VulkanBuffer
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memRequirements);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device.getDevice(), buffer, bufferMemory, 0);
}

void VulkanBuffer::copyBuffer(std::weak_ptr<VulkanCommandBuffer> command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer;

    if(auto tmp = command.lock())
    {
        commandBuffer=tmp->beginSingleTimeCommands();
    }
    else
    {
        VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
    }

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    if(auto tmp = command.lock())
    {
        tmp->endSingleTimeCommands(commandBuffer);
    }
    else
    {
        VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
    }
}


// AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
// {
//     // allocate buffer
//     VkBufferCreateInfo bufferInfo = {};
//     bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//     bufferInfo.pNext = nullptr;
//     bufferInfo.size = allocSize;

//     bufferInfo.usage = usage;

//     VmaAllocationCreateInfo vmaallocInfo = {};
//     vmaallocInfo.usage = memoryUsage;
//     vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
//     AllocatedBuffer newBuffer;

//     // allocate the buffer
//     VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation,
//                              &newBuffer.info));

//     return newBuffer;
// }

} // namespace Core