#include "VulkanSync.h"

namespace Core
{
    // Create the VulkanSync
    void VulkanSync::createSyncObjects(VkDevice device, const int max_frames_in_flight)
    {
        imageAvailableSemaphores.resize(max_frames_in_flight);
        renderFinishedSemaphores.resize(max_frames_in_flight);
        inFlightFences.resize(max_frames_in_flight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < max_frames_in_flight; i++)
        {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
            {
                VE_CORE_ERROR("failed to create synchronization objects for a frame!");
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
        VE_CORE_INFO("Created VulkanSync");
    }

    // Helper function to destroy the VulkanSync objects
    void VulkanSync::cleanup() 
    {
        for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        VE_CORE_INFO("Destroyed VulkanSync");
    }
} // namespace Core
