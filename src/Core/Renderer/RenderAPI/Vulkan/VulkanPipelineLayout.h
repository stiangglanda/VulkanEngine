#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>
#include "VulkanDescriptorSetLayout.h"

namespace Core
{

class VulkanPipelineLayout {
public:
    // Constructor to create the Pipeline Layout
    VulkanPipelineLayout(VkDevice device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts) : device(device), pipelineLayout(VK_NULL_HANDLE) 
    {
        createPipelineLayout(descriptorSetLayouts);
    }

    // Destructor to destroy the Pipeline Layout
    ~VulkanPipelineLayout() 
    {
        cleanup();
    }

    // Move constructor
    VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept 
        : device(other.device), pipelineLayout(other.pipelineLayout) 
    {
        other.pipelineLayout = VK_NULL_HANDLE; // Avoid double destruction
    }

    // Move assignment operator
    VulkanPipelineLayout& operator=(VulkanPipelineLayout&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            device = other.device;
            pipelineLayout = other.pipelineLayout;
            other.pipelineLayout = VK_NULL_HANDLE; // Avoid double destruction
        }
        return *this;
    }

    // Deleted copy constructor and assignment operator to prevent copying
    VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;
    VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;

    // Get the Pipeline Layout
    VkPipelineLayout get_handle() const
    {
        return pipelineLayout;
    }

private:
    VkDevice device;
    VkPipelineLayout pipelineLayout;

    // Create the Pipeline Layout
    void createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);

    void cleanup();
};

} // namespace Core
