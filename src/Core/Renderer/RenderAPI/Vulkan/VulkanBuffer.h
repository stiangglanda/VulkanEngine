#pragma once

#include "../../../vepch.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.h"
#include "VulkanResource.h"
#include <vulkan/vulkan.h>

namespace Core
{
class VulkanBuffer;

struct BufferBuilder : public VulkanMemoryManager::Builder<BufferBuilder, VkBufferCreateInfo>
{
    BufferBuilder(VkDeviceSize size)
        : Builder(VkBufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size})
    {
    }

    BufferBuilder &with_flags(VkBufferCreateFlags flags)
    {
        create_info.flags = flags;
        return *this;
    }

    BufferBuilder &with_usage(VkBufferUsageFlags usage)
    {
        create_info.usage = usage;
        return *this;
    }

    BufferBuilder &with_sharing_mode(VkSharingMode sharing_mode)
    {
        create_info.sharingMode = sharing_mode;
        return *this;
    }

    BufferBuilder &with_implicit_sharing_mode()
    {
        if (create_info.queueFamilyIndexCount != 0)
        {
            create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        }
        return *this;
    }

    VulkanBuffer build(VulkanDevice &device) const;
    std::unique_ptr<VulkanBuffer> build_unique(VulkanDevice &device) const;
};

class VulkanBuffer : public VulkanMemoryManager::VulkanMemoryManager<VkBuffer>
{
  public:
    static VulkanBuffer create_staging_buffer(VulkanDevice &device, VkDeviceSize size, const void *data);

    template <typename T> static VulkanBuffer create_staging_buffer(VulkanDevice &device, const std::vector<T> &data)
    {
        return create_staging_buffer(device, data.size() * sizeof(T), data.data());
    }

    template <typename T> static VulkanBuffer create_staging_buffer(VulkanDevice &device, const T &data)
    {
        return create_staging_buffer(device, sizeof(T), &data);
    }

    static void copyBuffer(std::weak_ptr<VulkanCommandBuffer> command, VkBuffer srcBuffer, VkBuffer dstBuffer,
                           VkDeviceSize size);

    /**
     * @brief Creates a buffer using VMA
     * @param device A valid Vulkan device
     * @param size The size in bytes of the buffer
     * @param buffer_usage The usage flags for the VkBuffer
     * @param memory_usage The memory usage of the buffer
     * @param flags The allocation create flags
     * @param queue_family_indices optional queue family indices
     */
    // [[deprecated]]
    VulkanBuffer(VulkanDevice &device, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage,
                 VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                                  VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                 const std::vector<uint32_t> &queue_family_indices = {});

    VulkanBuffer(VulkanDevice &device, const BufferBuilder &builder);

    VulkanBuffer(const VulkanBuffer &) = delete;

    VulkanBuffer(VulkanBuffer &&other) noexcept;

    ~VulkanBuffer();

    VulkanBuffer &operator=(const VulkanBuffer &) = delete;

    VulkanBuffer &operator=(VulkanBuffer &&) = delete;

    // FIXME should include a stride parameter, because if you want to copy out of a
    // uniform buffer that's dynamically indexed, you need to account for the aligned
    // offset of the T values
    template <typename T>
    static std::vector<T> copy(std::unordered_map<std::string, VulkanBuffer> &buffers, const char *buffer_name)
    {
        auto iter = buffers.find(buffer_name);
        if (iter == buffers.cend())
        {
            return {};
        }
        auto &buffer = iter->second;
        std::vector<T> out;

        const size_t sz = buffer.get_size();
        out.resize(sz / sizeof(T));
        const bool already_mapped = buffer.get_data() != nullptr;
        if (!already_mapped)
        {
            buffer.map();
        }
        memcpy(&out[0], buffer.get_data(), sz);
        if (!already_mapped)
        {
            buffer.unmap();
        }
        return out;
    }

    /**
     * @return The size of the buffer
     */
    VkDeviceSize get_size() const;

    /**
     * @return Return the buffer's device address (note: requires that the buffer has been created with the
     * VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT usage fla)
     */
    // uint64_t get_device_address() const;

  private:
    VkDeviceSize size{0};
};
} // namespace Core
