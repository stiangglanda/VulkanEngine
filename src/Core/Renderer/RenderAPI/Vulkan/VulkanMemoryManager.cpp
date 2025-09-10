#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "VulkanMemoryManager.h"

namespace Core
{

namespace VulkanMemoryManager
{

VmaAllocator &get_memory_allocator()
{
    static VmaAllocator memory_allocator = VK_NULL_HANDLE;
    return memory_allocator;
}

void init(const VulkanDevice &device, const VkInstance instance)
{
    // rework VulkanDevice and VulkanInstance so that this is possible
    //  allocator_info.physicalDevice   = static_cast<VkPhysicalDevice>(device.get_gpu().get_handle());
    //  allocator_info.device           = static_cast<VkDevice>(device.get_handle());
    //  allocator_info.instance         = static_cast<VkInstance>(device.get_gpu().get_instance().get_handle());
    VmaVulkanFunctions vma_vulkan_func{};
    vma_vulkan_func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vma_vulkan_func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.pVulkanFunctions = &vma_vulkan_func;
    allocator_info.physicalDevice = device.getPhysicalDevice();
    allocator_info.device = device.getDevice();
    allocator_info.instance = instance;
    allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    // TODO add actual extension system so thath this would work
    //  bool can_get_memory_requirements =
    //  device.is_extension_supported(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME); bool has_dedicated_allocation =
    //  device.is_extension_supported(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);

    // if (can_get_memory_requirements && has_dedicated_allocation &&
    // device.is_enabled(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME))
    // {
    // 	allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    // }

    // if (device.is_extension_supported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) &&
    // device.is_enabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
    // {
    // 	allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    // }

    // if (device.is_extension_supported(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) &&
    // device.is_enabled(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
    // {
    // 	allocator_info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    // }

    // if (device.is_extension_supported(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME) &&
    // device.is_enabled(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
    // {
    // 	allocator_info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    // }

    // if (device.is_extension_supported(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME) &&
    // device.is_enabled(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME))
    // {
    // 	allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    // }

    // if (device.is_extension_supported(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME) &&
    // device.is_enabled(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
    // {
    // 	allocator_info.flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
    // }

    init(allocator_info);
}

void init(const VmaAllocatorCreateInfo &create_info)
{
    VkResult result = vmaCreateAllocator(&create_info, &get_memory_allocator());
    if (result != VK_SUCCESS)
    {
        VE_CORE_ERROR("Cannot create allocator");
    }
    else
    {
        VE_CORE_INFO("Created VMA Allocator");
    }
}

void shutdown()
{
    auto &allocator = get_memory_allocator();
    if (allocator != VK_NULL_HANDLE)
    {
        VmaTotalStatistics stats;
        vmaCalculateStatistics(allocator, &stats);
        VE_CORE_INFO("Total device memory leaked: {} bytes.", stats.total.statistics.allocationBytes);
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
        VE_CORE_INFO("Destroyed VMA Allocator");
    }
}

VulkanMemoryManagerBase::VulkanMemoryManagerBase(const VmaAllocationCreateInfo &alloc_create_info)
    : alloc_create_info(alloc_create_info)
{
}

VulkanMemoryManagerBase::VulkanMemoryManagerBase(VulkanMemoryManagerBase &&other) noexcept
    : alloc_create_info(std::exchange(other.alloc_create_info, {})), allocation(std::exchange(other.allocation, {})),
      mapped_data(std::exchange(other.mapped_data, {})), coherent(std::exchange(other.coherent, {})),
      persistent(std::exchange(other.persistent, {}))
{
}

const uint8_t *VulkanMemoryManagerBase::get_data() const
{
    return mapped_data;
}

VkDeviceMemory VulkanMemoryManagerBase::get_memory() const
{
    VmaAllocationInfo alloc_info;
    vmaGetAllocationInfo(get_memory_allocator(), allocation, &alloc_info);
    return alloc_info.deviceMemory;
}

void VulkanMemoryManagerBase::flush(VkDeviceSize offset, VkDeviceSize size)
{
    if (!coherent)
    {
        vmaFlushAllocation(get_memory_allocator(), allocation, offset, size);
    }
}

uint8_t *VulkanMemoryManagerBase::map()
{
    if (!persistent && !mapped())
    {
        if (vmaMapMemory(get_memory_allocator(), allocation, reinterpret_cast<void **>(&mapped_data)) != VK_SUCCESS)
        {
            VE_CORE_ERROR("VulkanMemoryManagerBase::map error");
        }
        assert(mapped_data);
    }
    return mapped_data;
}

void VulkanMemoryManagerBase::unmap()
{
    if (!persistent && mapped())
    {
        vmaUnmapMemory(get_memory_allocator(), allocation);
        mapped_data = nullptr;
    }
}

size_t VulkanMemoryManagerBase::update(const uint8_t *data, size_t size, size_t offset)
{
    if (persistent)
    {
        std::copy(data, data + size, mapped_data + offset);
        flush();
    }
    else
    {
        map();
        std::copy(data, data + size, mapped_data + offset);
        flush();
        unmap();
    }
    return size;
}

size_t VulkanMemoryManagerBase::update(void const *data, size_t size, size_t offset)
{
    return update(reinterpret_cast<const uint8_t *>(data), size, offset);
}

void VulkanMemoryManagerBase::post_create(VmaAllocationInfo const &allocation_info)
{
    VkMemoryPropertyFlags memory_properties;
    vmaGetAllocationMemoryProperties(get_memory_allocator(), allocation, &memory_properties);
    coherent = (memory_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    mapped_data = static_cast<uint8_t *>(allocation_info.pMappedData);
    persistent = mapped();
}

[[nodiscard]] VkBuffer VulkanMemoryManagerBase::create_buffer(VkBufferCreateInfo const &create_info)
{
    VkBuffer handleResult = VK_NULL_HANDLE;
    VmaAllocationInfo allocation_info{};

    auto result = vmaCreateBuffer(get_memory_allocator(), &create_info, &alloc_create_info, &handleResult, &allocation,
                                  &allocation_info);

    if (result != VK_SUCCESS)
    {
        VE_CORE_ERROR("Cannot create Buffer");
    }
    post_create(allocation_info);
    return handleResult;
}

[[nodiscard]] VkImage VulkanMemoryManagerBase::create_image(VkImageCreateInfo const &create_info)
{
    assert(0 < create_info.mipLevels && "Images should have at least one level");
    assert(0 < create_info.arrayLayers && "Images should have at least one layer");
    assert(0 < create_info.usage && "Images should have at least one usage type");

    VkImage handleResult = VK_NULL_HANDLE;
    VmaAllocationInfo allocation_info{};

#if 0
		// If the image is an attachment, prefer dedicated memory
		constexpr VkImageUsageFlags attachment_only_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		if (create_info.usage & attachment_only_flags)
		{
			alloc_create_info.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}

		if (create_info.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
		{
			alloc_create_info.preferredFlags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
		}
#endif

    auto result = vmaCreateImage(get_memory_allocator(), &create_info, &alloc_create_info, &handleResult, &allocation,
                                 &allocation_info);

    if (result != VK_SUCCESS)
    {
        VE_CORE_ERROR("Cannot create Image");
    }

    post_create(allocation_info);
    return handleResult;
}

void VulkanMemoryManagerBase::destroy_buffer(VkBuffer handle)
{
    if (handle != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE)
    {
        unmap();
        vmaDestroyBuffer(get_memory_allocator(), handle, allocation);
        clear();
    }
}

void VulkanMemoryManagerBase::destroy_image(VkImage image)
{
    if (image != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE)
    {
        unmap();
        vmaDestroyImage(get_memory_allocator(), image, allocation);
        clear();
    }
}

bool VulkanMemoryManagerBase::mapped() const
{
    return mapped_data != nullptr;
}

void VulkanMemoryManagerBase::clear()
{
    mapped_data = nullptr;
    persistent = false;
    alloc_create_info = {};
}

} // namespace VulkanMemoryManager
} // namespace Core
