#pragma once
#include "../../../vepch.h"
#include <volk.h>
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace Core
{

class VulkanRenderPass {
public:
    VulkanRenderPass(VulkanDevice& device, VulkanSwapChain& swapChain) : device(device.getDevice()), renderPass(VK_NULL_HANDLE) 
    {
        createRenderPass(device, swapChain);
    }

    ~VulkanRenderPass() 
    {
        cleanup();
    }

    VulkanRenderPass(VulkanRenderPass&& other) noexcept 
        : device(other.device), renderPass(other.renderPass) 
    {
        other.renderPass = VK_NULL_HANDLE;
    }

    VulkanRenderPass& operator=(VulkanRenderPass&& other) noexcept {
        if (this != &other) {
            cleanup();
            device = other.device;
            renderPass = other.renderPass;
            other.renderPass = VK_NULL_HANDLE;
        }
        return *this;
    }

    VulkanRenderPass(const VulkanRenderPass&) = delete;
    VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

    VkRenderPass get_handle() const
    {
        return renderPass;
    }

private:
    VkDevice device;
    VkRenderPass renderPass;

    void createRenderPass(VulkanDevice& device, VulkanSwapChain& swapChain);

    void cleanup();
};

} // namespace Core
