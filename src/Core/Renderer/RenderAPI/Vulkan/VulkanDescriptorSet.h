#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>
#include "VulkanBuffer.h"
#include "VulkanImage.h"


namespace Core
{

class VulkanDescriptorSet {
public:
    // Constructor to create the descriptor set
    VulkanDescriptorSet(VkDevice device, const int max_frames_in_flight, 
                        VkDescriptorSetLayout descriptorSetLayout, 
                        const std::vector<std::unique_ptr<VulkanBuffer>>& uniformBuffers, 
                        std::weak_ptr<VulkanImage> texture) : device(device), descriptorPool(VK_NULL_HANDLE) 
    {
        createDescriptorPool(max_frames_in_flight);
        createDescriptorSets(max_frames_in_flight, descriptorSetLayout, uniformBuffers, texture);
    }

    // Destructor to destroy the descriptor set
    ~VulkanDescriptorSet() {
        cleanup();
    }

    // Deleted copy constructor and assignment operator to prevent copying
    VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
    VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;

    // Get the descriptor set
    VkDescriptorSet* get_handle_ptr_at_index(unsigned int index)
    {
        if(index<descriptorSets.size())
        {
            return &descriptorSets[index];
        }
        else
        {
            VE_CORE_ERROR("VulkanDescriptorSet::get_handle_at_index: index out of bounds");
            return nullptr;
        }
    }

private:
    VkDevice device;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    // Create the descriptor pool
    void createDescriptorPool(const int max_frames_in_flight);

    // Create the descriptor sets
    void createDescriptorSets(const int max_frames_in_flight, 
                            VkDescriptorSetLayout descriptorSetLayout, 
                            const std::vector<std::unique_ptr<VulkanBuffer>>& uniformBuffers, 
                            std::weak_ptr<VulkanImage> texture);

    // Helper function to destroy the descriptor set
    void cleanup();
};

} // namespace Core
