#include "VulkanDescriptorSet.h"
#include "VulkanDebug.h"
#include "UniformBufferObject.h"

namespace Core
{
    void VulkanDescriptorSet::createDescriptorPool(const int max_frames_in_flight)
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(max_frames_in_flight);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(max_frames_in_flight);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(max_frames_in_flight);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            VE_CORE_ERROR("failed to create descriptor pool!");
            throw std::runtime_error("failed to create descriptor pool!");
        }
        else
        {
            VE_CORE_INFO("Created Descriptor Pool");
        }
    }

    // Create the descriptor set
    void VulkanDescriptorSet::createDescriptorSets(const int max_frames_in_flight, 
                                                   VkDescriptorSetLayout descriptorSetLayout, 
                                                   const std::vector<std::unique_ptr<VulkanBuffer>>& uniformBuffers, 
                                                   std::weak_ptr<VulkanImage> texture)
    {
        std::vector<VkDescriptorSetLayout> layouts(max_frames_in_flight, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(max_frames_in_flight);
        allocInfo.pSetLayouts = layouts.data();
        descriptorSets.resize(max_frames_in_flight);

        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            VE_CORE_ERROR("failed to allocate descriptor sets!");
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < max_frames_in_flight; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i]->get_handle();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            if (auto tmp = texture.lock())
            {
                imageInfo.imageView = tmp->getImageView();
                imageInfo.sampler = tmp->getSampler();
            }
            else
            {
                VE_CORE_ERROR("VulkanDescriptorSet::createDescriptorSets VulkanImage ptr expired");
            }

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
        }
        VE_CORE_INFO("Create Descriptor Sets");
    }

    // Helper function to destroy the descriptor set layout
    void VulkanDescriptorSet::cleanup() 
    {
        if (descriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(device, descriptorPool, nullptr);
            descriptorPool = VK_NULL_HANDLE;
            VE_CORE_INFO("Successful: Destroyed Descriptor Pool");
        }
    }
} // namespace Core
