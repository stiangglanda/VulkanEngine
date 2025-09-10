#include "VulkanBuffer.h"

namespace Core
{

VulkanBuffer BufferBuilder::build(VulkanDevice &device) const
{
    return VulkanBuffer(device, *this);
}

std::unique_ptr<VulkanBuffer> BufferBuilder::build_unique(VulkanDevice &device) const
{
    return std::make_unique<VulkanBuffer>(device, *this);
}

VulkanBuffer VulkanBuffer::create_staging_buffer(VulkanDevice &device, VkDeviceSize size, const void *data)
{
    BufferBuilder builder{size};
    builder.with_vma_flags(VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    builder.with_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    VulkanBuffer result(device, builder);
    if (data != nullptr)
    {
        result.update(data, size);
    }
    return result;
}

VulkanBuffer::VulkanBuffer(VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags buffer_usage,
                           VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags,
                           const std::vector<uint32_t> &queue_family_indices)
    : VulkanBuffer(device, BufferBuilder(size)
                               .with_usage(buffer_usage)
                               .with_vma_usage(memory_usage)
                               .with_vma_flags(flags)
                               .with_queue_families(queue_family_indices)
                               .with_implicit_sharing_mode())
{
}

VulkanBuffer::VulkanBuffer(VulkanDevice &device, const BufferBuilder &builder)
    : VulkanMemoryManager{builder.alloc_create_info, VK_NULL_HANDLE, &device}, size(builder.create_info.size)
{
    set_handle(create_buffer(builder.create_info));
    if (!builder.debug_name.empty())
    {
        // set_debug_name(builder.debug_name);//TODO maybe implement debug name in VulkanResource
    }
    VE_CORE_INFO("VulkanBuffer created buffer");
}

VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept
    : VulkanMemoryManager{std::move(other)}, size{std::exchange(other.size, {})}
{
}

VulkanBuffer::~VulkanBuffer()
{
    destroy_buffer(get_handle());
    VE_CORE_INFO("VulkanBuffer destroyed buffer");
}

VkDeviceSize VulkanBuffer::get_size() const
{
    return size;
}

void VulkanBuffer::copyBuffer(std::weak_ptr<VulkanCommandBuffer> command, VkBuffer srcBuffer, VkBuffer dstBuffer,
                              VkDeviceSize size)
{
    VkCommandBuffer commandBuffer;

    if (auto tmp = command.lock())
    {
        commandBuffer = tmp->beginSingleTimeCommands();
    }
    else
    {
        VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
    }

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    if (auto tmp = command.lock())
    {
        tmp->endSingleTimeCommands(commandBuffer);
    }
    else
    {
        VE_CORE_ERROR("VulkanBuffer::copyBuffer VulkanCommandBuffer ptr expired");
    }
}

} // namespace Core
