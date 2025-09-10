#pragma once
#include "../../../vepch.h"
#include <volk.h>

namespace Core
{

class VulkanSync {
public:
    VulkanSync(VkDevice device, const int max_frames_in_flight) : device(device) 
    {
        createSyncObjects(device, max_frames_in_flight);
    }

    ~VulkanSync() 
    {
        cleanup();
    }

    VulkanSync(VulkanSync&& other) noexcept 
        : device(other.device) 
    {
    }

    VulkanSync& operator=(VulkanSync&& other) noexcept {
        if (this != &other) {
            cleanup(); // Destroy current layout if it exists
            device = other.device;
        }
        return *this;
    }

    VulkanSync(const VulkanSync&) = delete;
    VulkanSync& operator=(const VulkanSync&) = delete;

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

    void createSyncObjects(VkDevice device, const int max_frames_in_flight);

    void cleanup();
};

} // namespace Core
