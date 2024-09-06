#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core
{

class VulkanDescriptorSetLayout {
public:
    // Constructor to create the descriptor set layout
    VulkanDescriptorSetLayout(VkDevice device) : device(device), descriptorSetLayout(VK_NULL_HANDLE) {
        createDescriptorSetLayout();
    }

    // Destructor to destroy the descriptor set layout
    ~VulkanDescriptorSetLayout() {
        cleanup();
    }

    // Move constructor
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& other) noexcept 
        : device(other.device), descriptorSetLayout(other.descriptorSetLayout) {
        other.descriptorSetLayout = VK_NULL_HANDLE; // Avoid double destruction
    }

    // Move assignment operator
    VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            device = other.device;
            descriptorSetLayout = other.descriptorSetLayout;
            other.descriptorSetLayout = VK_NULL_HANDLE; // Avoid double destruction
        }
        return *this;
    }

    // Deleted copy constructor and assignment operator to prevent copying
    VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

    // Get the descriptor set layout
    VkDescriptorSetLayout get_handle() const
    {
        return descriptorSetLayout;
    }

    // const VkDescriptorSetLayout* get_handle_ptr() const//TODO come up with a solution so i can delete this function
    // {                                                  //this can be deleted if i support multiple discription set layouts
    //     return &descriptorSetLayout;
    // }

private:
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;

    // Create the descriptor set layout
    void createDescriptorSetLayout();

    // Helper function to destroy the descriptor set layout
    void cleanup();
};



} // namespace Core
