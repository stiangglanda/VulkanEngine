#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>
#include "VulkanDescriptorSetLayout.h"

namespace Core
{

class VulkanPipelineLayout {
public:
    // Constructor to create the descriptor set layout
    VulkanPipelineLayout(VkDevice device, std::weak_ptr<VulkanDescriptorSetLayout> descriptorSetLayout) : device(device), pipelineLayout(VK_NULL_HANDLE) 
    {
        createPipelineLayout(descriptorSetLayout);
    }

    // Destructor to destroy the descriptor set layout
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

    // Get the descriptor set layout
    VkPipelineLayout get_handle() const
    {
        return pipelineLayout;
    }

private:
    VkDevice device;
    VkPipelineLayout pipelineLayout;

    // Create the descriptor set layout
    void createPipelineLayout(std::weak_ptr<VulkanDescriptorSetLayout> descriptorSetLayout);

    // Helper function to destroy the descriptor set layout
    void cleanup();
};

} // namespace Core
