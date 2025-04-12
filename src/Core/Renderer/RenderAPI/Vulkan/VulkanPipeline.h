#pragma once
#include "../../../vepch.h"
#include <volk.h>
#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"

namespace Core
{

class VulkanPipeline {
public:
    VulkanPipeline(VulkanDevice &device, VkDescriptorSetLayout descriptorSetLayout, VkFormat color_attachment_format)
        : device(device), pipeline(VK_NULL_HANDLE) // questianable
    {
        createPipeline(descriptorSetLayout, color_attachment_format);
    }

    ~VulkanPipeline() {
        cleanup();
    }

    // Move constructor
    VulkanPipeline(VulkanPipeline&& other) noexcept 
        : device(other.device), pipeline(other.pipeline) {
        other.pipeline = VK_NULL_HANDLE; // Avoid double destruction
    }

    // Move assignment operator
    VulkanPipeline& operator=(VulkanPipeline&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            pipeline = other.pipeline;
            pipelineLayout = std::move(other.pipelineLayout);
            other.pipeline = VK_NULL_HANDLE; // Avoid double destruction
        }
        return *this;
    }

    // Deleted copy constructor and assignment operator to prevent copying
    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    // Get the descriptor set layout
    VkPipeline get_handle() const
    {
        return pipeline;
    }

    VkPipelineLayout get_PipelineLayout() const
    {
        return pipelineLayout->get_handle();
    }

private:
    VulkanDevice& device;
    VkPipeline pipeline;
    std::unique_ptr<VulkanPipelineLayout> pipelineLayout;

    // Create the descriptor set layout
    void createPipeline(VkDescriptorSetLayout descriptorSetLayout, VkFormat color_attachment_format);

    // Helper function to destroy the descriptor set layout
    void cleanup();
};

} // namespace Core
