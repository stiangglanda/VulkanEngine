#pragma once
#include "../../../vepch.h"
#include <volk.h>
#include "VulkanDescriptorSetLayout.h"

namespace Core
{

class VulkanPipelineLayout {
public:
    VulkanPipelineLayout(VkDevice device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts) : device(device), pipelineLayout(VK_NULL_HANDLE) 
    {
        createPipelineLayout(descriptorSetLayouts);
    }

    ~VulkanPipelineLayout() 
    {
        cleanup();
    }

    VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept 
        : device(other.device), pipelineLayout(other.pipelineLayout) 
    {
        other.pipelineLayout = VK_NULL_HANDLE;
    }

    VulkanPipelineLayout& operator=(VulkanPipelineLayout&& other) noexcept {
        if (this != &other) {
            cleanup();
            device = other.device;
            pipelineLayout = other.pipelineLayout;
            other.pipelineLayout = VK_NULL_HANDLE;
        }
        return *this;
    }

    VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;
    VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;

    VkPipelineLayout get_handle() const
    {
        return pipelineLayout;
    }

private:
    VkDevice device;
    VkPipelineLayout pipelineLayout;

    void createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);

    void cleanup();
};

} // namespace Core
