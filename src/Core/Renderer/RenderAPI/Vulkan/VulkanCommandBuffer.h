#pragma once
#include "../../../vepch.h"
#include "VulkanDevice.h"
#include <volk.h>

namespace Core
{
class VulkanCommandBuffer
{
  public:
    VulkanCommandBuffer(const VulkanDevice &device) : device(device)
    {
    }
    ~VulkanCommandBuffer()
    {
    }

    void Init(VkSurfaceKHR surface, const int max_frames_in_flight);
    void Shutdown();

    VkCommandBuffer getCommandBuffer(uint32_t index) const
    {
        if (commandBuffers.size() > index)
        {
            return commandBuffers[index];
        }
        else
        {
            VE_CORE_ERROR("VulkanCommandBuffer.getCommandBuffer index out of bounds");
            return nullptr;
        }
    }

    const VkCommandBuffer beginSingleTimeCommands() const;
    const void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    void begin(uint32_t currentFrame, VkCommandBufferUsageFlags flags = 0);
    void end(uint32_t currentFrame);
    void submit(uint32_t currentFrame, VkFence fence = VK_NULL_HANDLE, VkSemaphore waitSemaphore = VK_NULL_HANDLE,
                VkSemaphore signalSemaphore = VK_NULL_HANDLE,
                VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    void ResetCommandBuffer(uint32_t currentFrame, VkCommandBufferResetFlags flags = 0);
    void createCommandPool(VkSurfaceKHR surface, const int max_frames_in_flight);
    void createCommandBuffers(const int max_frames_in_flight);

  private:
    const VulkanDevice &device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
} // namespace Core
