#pragma once
#include "../../../vepch.h"
#include <volk.h>
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace Core
{

class VulkanRenderPass {
public:
    // Constructor to create the Render Pass
    VulkanRenderPass(VulkanDevice& device, VulkanSwapChain& swapChain) : device(device.getDevice()), renderPass(VK_NULL_HANDLE) 
    {
        createRenderPass(device, swapChain);
    }

    // Destructor to destroy the Render Pass
    ~VulkanRenderPass() 
    {
        cleanup();
    }

    // Move constructor
    VulkanRenderPass(VulkanRenderPass&& other) noexcept 
        : device(other.device), renderPass(other.renderPass) 
    {
        other.renderPass = VK_NULL_HANDLE; // Avoid double destruction
    }

    // Move assignment operator
    VulkanRenderPass& operator=(VulkanRenderPass&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            device = other.device;
            renderPass = other.renderPass;
            other.renderPass = VK_NULL_HANDLE; // Avoid double destruction
        }
        return *this;
    }

    // Deleted copy constructor and assignment operator to prevent copying
    VulkanRenderPass(const VulkanRenderPass&) = delete;
    VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

    // Get the Render Pass
    VkRenderPass get_handle() const
    {
        return renderPass;
    }

private:
    VkDevice device;
    VkRenderPass renderPass;

    // Create the Render Pass
    void createRenderPass(VulkanDevice& device, VulkanSwapChain& swapChain);

    void cleanup();
};

} // namespace Core
