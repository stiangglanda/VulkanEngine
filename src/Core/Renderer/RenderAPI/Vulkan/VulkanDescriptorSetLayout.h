#pragma once
#include "../../../vepch.h"
#include <volk.h>

namespace Core
{

class VulkanDescriptorSetLayout {
public:
    VulkanDescriptorSetLayout(VkDevice device) : device(device), descriptorSetLayout(VK_NULL_HANDLE) {
        createDescriptorSetLayout();
    }

    ~VulkanDescriptorSetLayout() {
        cleanup();
    }

    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& other) noexcept 
        : device(other.device), descriptorSetLayout(other.descriptorSetLayout) {
        other.descriptorSetLayout = VK_NULL_HANDLE;
    }

    VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            device = other.device;
            descriptorSetLayout = other.descriptorSetLayout;
            other.descriptorSetLayout = VK_NULL_HANDLE;
        }
        return *this;
    }

    VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

    VkDescriptorSetLayout get_handle() const
    {
        return descriptorSetLayout;
    }

private:
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;

    void createDescriptorSetLayout();

    void cleanup();
};

} // namespace Core
