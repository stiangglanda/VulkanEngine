#pragma once
#include <volk.h>
#include "../../../vepch.h"
#include "VulkanResource.h"
#include <vk_mem_alloc.h>


namespace Core
{

namespace VulkanMemoryManager
{

template <typename BuilderType, typename CreateInfoType, typename SharingModeType = VkSharingMode> struct Builder
{
    VmaAllocationCreateInfo alloc_create_info{};
    std::string debug_name;
    CreateInfoType create_info;

  protected:
    Builder(const Builder &other) = delete;
    Builder(const CreateInfoType &create_info) : create_info(create_info)
    {
        alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    };

  public:
    BuilderType &with_debug_name(const std::string &name)
    {
        debug_name = name;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_vma_usage(VmaMemoryUsage usage)
    {
        alloc_create_info.usage = usage;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_vma_flags(VmaAllocationCreateFlags flags)
    {
        alloc_create_info.flags = flags;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_vma_required_flags(VkMemoryPropertyFlags flags)
    {
        alloc_create_info.requiredFlags = flags;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_vma_preferred_flags(VkMemoryPropertyFlags flags)
    {
        alloc_create_info.preferredFlags = flags;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_memory_type_bits(uint32_t type_bits)
    {
        alloc_create_info.memoryTypeBits = type_bits;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_vma_pool(VmaPool pool)
    {
        alloc_create_info.pool = pool;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_queue_families(uint32_t count, const uint32_t *family_indices)
    {
        create_info.queueFamilyIndexCount = count;
        create_info.pQueueFamilyIndices = family_indices;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_sharing(VkSharingMode sharing)
    {
        create_info.sharingMode = sharing;
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_implicit_sharing_mode()
    {
        if (create_info.queueFamilyIndexCount != 0)
        {
            create_info.sharingMode = static_cast<SharingModeType>(VK_SHARING_MODE_CONCURRENT);
        }
        else
        {
            create_info.sharingMode = static_cast<SharingModeType>(VK_SHARING_MODE_EXCLUSIVE);
        }
        return *static_cast<BuilderType *>(this);
    }

    BuilderType &with_queue_families(const std::vector<uint32_t> &queue_families)
    {
        return with_queue_families(static_cast<uint32_t>(queue_families.size()), queue_families.data());
    }
};

void init(const VmaAllocatorCreateInfo &create_info);

void init(const VulkanDevice &device, const VkInstance instance);

VmaAllocator &get_memory_allocator();

void shutdown();

class VulkanMemoryManagerBase
{
  public:
    VulkanMemoryManagerBase() = default;
    VulkanMemoryManagerBase(const VmaAllocationCreateInfo &alloc_create_info);
    VulkanMemoryManagerBase(VulkanMemoryManagerBase const &) = delete;
    VulkanMemoryManagerBase(VulkanMemoryManagerBase &&other) noexcept;

    VulkanMemoryManagerBase &operator=(const VulkanMemoryManagerBase &) = delete;
    VulkanMemoryManagerBase &operator=(VulkanMemoryManagerBase &&) = default;

    const uint8_t *get_data() const;
    VkDeviceMemory get_memory() const;

    /**
     * @brief Flushes memory if it is HOST_VISIBLE and not HOST_COHERENT
     */
    void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

    /**
     * @brief Returns true if the memory is mapped, false otherwise
     * @return mapping status
     */
    bool mapped() const;

    /**
     * @brief Maps vulkan memory if it isn't already mapped to an host visible address
     * @return Pointer to host visible memory
     */
    uint8_t *map();

    /**
     * @brief Unmaps vulkan memory from the host visible address
     */
    void unmap();

    /**
     * @brief Copies byte data into the buffer
     * @param data The data to copy from
     * @param size The amount of bytes to copy
     * @param offset The offset to start the copying into the mapped data
     */
    size_t update(const uint8_t *data, size_t size, size_t offset = 0);
    /**
     * @brief Converts any non byte data into bytes and then updates the buffer
     * @param data The data to copy from
     * @param size The amount of bytes to copy
     * @param offset The offset to start the copying into the mapped data
     */
    size_t update(void const *data, size_t size, size_t offset = 0);

    /**
     * @todo Use the vk::ArrayBuffer class to collapse some of these templates
     * @brief Copies a vector of items into the buffer
     * @param data The data vector to upload
     * @param offset The offset to start the copying into the mapped data
     */
    template <typename T> size_t update(std::vector<T> const &data, size_t offset = 0)
    {
        return update(data.data(), data.size() * sizeof(T), offset);
    }

    template <typename T, size_t N> size_t update(std::array<T, N> const &data, size_t offset = 0)
    {
        return update(data.data(), data.size() * sizeof(T), offset);
    }

    /**
     * @brief Copies an object as byte data into the buffer
     * @param object The object to convert into byte data
     * @param offset The offset to start the copying into the mapped data
     */
    template <class T> size_t convert_and_update(const T &object, size_t offset = 0)
    {
        return update(reinterpret_cast<const uint8_t *>(&object), sizeof(T), offset);
    }

  protected:
    virtual void post_create(VmaAllocationInfo const &allocation_info);
    [[nodiscard]] VkBuffer create_buffer(VkBufferCreateInfo const &create_info);
    [[nodiscard]] VkImage create_image(VkImageCreateInfo const &create_info);
    void destroy_buffer(VkBuffer buffer);
    void destroy_image(VkImage image);
    void clear();

    VmaAllocationCreateInfo alloc_create_info{};
    VmaAllocation allocation = VK_NULL_HANDLE;
    uint8_t *mapped_data = nullptr;
    bool coherent = false;
    bool persistent = false; // Whether the buffer is persistently mapped or not
};

template <typename HandleType, // TODO maybe not nessesary
          typename ParentType = VulkanResource<HandleType>>
class VulkanMemoryManager : public ParentType, public VulkanMemoryManagerBase
{
  public:
    using ParentType::ParentType;

    VulkanMemoryManager() = delete;
    VulkanMemoryManager(const VulkanMemoryManager &) = delete;
    VulkanMemoryManager &operator=(VulkanMemoryManager const &other) = delete;
    VulkanMemoryManager &operator=(VulkanMemoryManager &&other) = default;

    // Import the base class constructors
    template <typename... Args>
    VulkanMemoryManager(const VmaAllocationCreateInfo &alloc_create_info, Args &&...args)
        : ParentType(std::forward<Args>(args)...), VulkanMemoryManagerBase(alloc_create_info)
    {
    }

    VulkanMemoryManager(VulkanMemoryManager &&other) noexcept
        : ParentType{static_cast<ParentType &&>(other)},
          VulkanMemoryManagerBase{static_cast<VulkanMemoryManagerBase &&>(other)}
    {
    }

    const HandleType *get() const
    {
        return &ParentType::get_handle();
    }
};

} // namespace VulkanMemoryManager
} // namespace Core
