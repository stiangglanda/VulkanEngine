#pragma once
#include "../../../vepch.h"
#include <vulkan/vulkan.h>

namespace Core
{

class VulkanSync {
public:
    // Constructor to create VulkanSync
    VulkanSync(VkDevice device, const int max_frames_in_flight) : device(device) 
    {
        createSyncObjects(device, max_frames_in_flight);
    }

    // Destructor to destroy the VulkanSync Objects
    ~VulkanSync() 
    {
        cleanup();
    }

    // Move constructor
    VulkanSync(VulkanSync&& other) noexcept 
        : device(other.device) 
    {
    }

    // Move assignment operator
    VulkanSync& operator=(VulkanSync&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            device = other.device;
        }
        return *this;
    }

    // Deleted copy constructor and assignment operator to prevent copying
    VulkanSync(const VulkanSync&) = delete;
    VulkanSync& operator=(const VulkanSync&) = delete;

    // Get the Semaphore by index
    VkSemaphore get_imageAvailableSemaphores_handle_by_index(unsigned int index) const
    {
        if(index<imageAvailableSemaphores.size())
        {
            return imageAvailableSemaphores[index];
        }
        else 
        {
            VE_CORE_INFO("VulkanSync imageAvailableSemaphores was out of bounds");
            return nullptr;
        }
    }

    // Get the Semaphore by index
    VkSemaphore* get_renderFinishedSemaphores_handle_by_index(unsigned int index)
    {
        if(index<renderFinishedSemaphores.size())
        {
            return &renderFinishedSemaphores[index];
        }
        else 
        {
            VE_CORE_INFO("VulkanSync renderFinishedSemaphores was out of bounds");
            return nullptr;
        }
    }

    // Get the Fence by index
    VkFence* get_inFlightFences_handle_by_index(unsigned int index)
    {
        if(index<inFlightFences.size())
        {
            return &inFlightFences[index];
        }
        else 
        {
            VE_CORE_INFO("VulkanSync inFlightFences was out of bounds");
            return nullptr;
        }
    }

private:
    VkDevice device;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    // Create the VulkanSync Objects
    void createSyncObjects(VkDevice device, const int max_frames_in_flight);

    void cleanup();
};

} // namespace Core
