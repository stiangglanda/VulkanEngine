#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>
#include "VulkanPipelineLayout.h"

namespace Core
{

class VulkanPipeline {
public:
    // Constructor to create the descriptor set layout
    VulkanPipeline(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass) : device(device), pipeline(VK_NULL_HANDLE) //questianable
    {
        createPipeline(descriptorSetLayout, renderPass);
    }

    // Destructor to destroy the descriptor set layout
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

    // const VkDescriptorSetLayout* get_handle_ptr() const//TODO come up with a solution so i can delete this function
    // {                                                  //this can be deleted if i support multiple discription set layouts
    //     return &descriptorSetLayout;
    // }

private:
    VkDevice device;
    VkPipeline pipeline;
    std::unique_ptr<VulkanPipelineLayout> pipelineLayout;

    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);

    // Create the descriptor set layout
    void createPipeline(VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass);

    // Helper function to destroy the descriptor set layout
    void cleanup();
};

} // namespace Core
