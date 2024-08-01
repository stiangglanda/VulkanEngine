#pragma once

#include <vulkan/vulkan.h>

namespace Core
{
/**
 * @brief Facade class around a Vulkan handle providing some utility functions.
 */
template <typename T>
class VulkanResource
{
public:
    VulkanResource() = default;

    explicit VulkanResource(T handle) :
        handle{handle}
    {}

    VulkanResource(const VulkanResource &) = delete;

    VulkanResource(VulkanResource &&other) noexcept :
        handle{other.handle}
    {
        other.handle = VK_NULL_HANDLE;
    }

    VulkanResource &operator=(const VulkanResource &) = delete;

    VulkanResource &operator=(VulkanResource &&other) noexcept
    {
        if (this != &other)
        {
            handle = other.handle;
            other.handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~VulkanResource() = default;

    T get_handle() const
    {
        return handle;
    }

    operator T() const
    {
        return handle;
    }

protected:
    T handle{VK_NULL_HANDLE};
};

} // namespace core

