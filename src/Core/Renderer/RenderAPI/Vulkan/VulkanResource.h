#pragma once

#include "VulkanDevice.h"

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

    VulkanResource(T handle = nullptr, VulkanDevice *device_ = nullptr);

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

    VulkanDevice       &get_device();
	VulkanDevice const &get_device() const;

	bool has_device() const;
	bool has_handle() const;

    void set_handle(T hdl);

    operator T() const
    {
        return handle;
    }

protected:
    T handle{VK_NULL_HANDLE};
    VulkanDevice *device;
};

template <typename T>
inline VulkanResource<T>::VulkanResource(T handle_, VulkanDevice *device_) :
    handle{handle_}
{
		device = device_;
}

template <typename T>
inline VulkanDevice& VulkanResource<T>::get_device()
{
	assert(device && "VKBDevice handle not set");
	return *device;
}

template <typename T>
inline VulkanDevice const &VulkanResource<T>::get_device() const
{
	assert(device && "VKBDevice handle not set");
	return *device;
}

template <typename T>
inline bool VulkanResource<T>::has_device() const
{
	return device != nullptr;
}

template <typename T>
inline bool VulkanResource<T>::has_handle() const
{
	return handle != nullptr;
}

template <typename T>
inline void VulkanResource<T>::set_handle(T hdl)
{
	handle = hdl;
}

} // namespace core

