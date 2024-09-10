#include "VulkanPipelineLayout.h"
#include "VulkanDebug.h"

namespace Core
{

// Create the Pipeline Layout
    void VulkanPipelineLayout::createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts) 
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            VE_CORE_ERROR("failed to create pipeline layout!");
            throw std::runtime_error("failed to create pipeline layout!");
        }
        else
        {
            VE_CORE_INFO("Created Pipeline Layout");
        }
    }

    // Helper function to destroy the Pipeline Layout
    void VulkanPipelineLayout::cleanup() {
        if (pipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            pipelineLayout = VK_NULL_HANDLE;
            VE_CORE_INFO("Destroyed Pipeline Layout");
        }
    }
} // namespace Core
