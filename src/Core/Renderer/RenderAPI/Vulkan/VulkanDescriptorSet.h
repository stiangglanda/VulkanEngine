#pragma once
#include "../../../vepch.h"
#include <volk.h>
#include "VulkanBuffer.h"
#include "VulkanImage.h"


namespace Core
{

class VulkanDescriptorSet {
public:
    VulkanDescriptorSet(VkDevice device, const int max_frames_in_flight, 
                        VkDescriptorSetLayout descriptorSetLayout, 
                        const std::vector<std::unique_ptr<VulkanBuffer>>& uniformBuffers, 
                        const VkBuffer VertexBuffer, 
                        size_t verticesSize,
                        std::weak_ptr<VulkanImage> texture) : device(device), descriptorPool(VK_NULL_HANDLE) 
    {
        createDescriptorPool(max_frames_in_flight);
        createDescriptorSets(max_frames_in_flight, descriptorSetLayout, uniformBuffers, VertexBuffer,
                             verticesSize, texture);
    }

    ~VulkanDescriptorSet() {
        cleanup();
    }

    VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
    VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;

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

    void createDescriptorPool(const int max_frames_in_flight);

    void createDescriptorSets(const int max_frames_in_flight, 
                            VkDescriptorSetLayout descriptorSetLayout, 
                            const std::vector<std::unique_ptr<VulkanBuffer>>& uniformBuffers, 
                            const VkBuffer VertexBuffer,
                            size_t verticesSize,
                            std::weak_ptr<VulkanImage> texture);

    void cleanup();
};

} // namespace Core
