#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>
#include "VulkanPipelineLayout.h"

namespace Core
{

class VulkanPipeline {
public:
    VulkanPipeline(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass) : device(device), pipeline(VK_NULL_HANDLE) //questianable
    {
        createPipeline(descriptorSetLayout, renderPass);
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
            device = other.device;
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
    VkDevice device;
    VkPipeline pipeline;
    std::unique_ptr<VulkanPipelineLayout> pipelineLayout;

    // VkShaderModule createShaderModule(const std::vector<char> &code);//TODO should be in VulkanShader
    // std::vector<char> readFile(const std::string &filename);//TODO should be in VulkanShader

    // Create the descriptor set layout
    void createPipeline(VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass);

    // Helper function to destroy the descriptor set layout
    void cleanup();
};

} // namespace Core
