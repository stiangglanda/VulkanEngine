#pragma once 
#include <vulkan/vulkan.h>
#include "../../../vepch.h"
#include "VulkanDevice.h"

namespace Core 
{

    struct SwapChainSupportDetails//move this to VulkanSwapChain imidiatly
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanSwapChain 
    {
    public:
        VulkanSwapChain() {}
        ~VulkanSwapChain() {}
        
        void Init(const VulkanDevice& device, VkSurfaceKHR surface);
        void createDepthResourcesAndFramebuffers(const VulkanDevice& device, VkRenderPass renderPass);
        void Shutdown(VkDevice device);
        static const SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        
        VkFormat getImageFormat() const
        {
            return swapChainImageFormat;
        }

        VkExtent2D getExtent() const
        {
            return swapChainExtent;
        }

        VkSwapchainKHR getSwapChain() const
        {
            return swapChain;
        }

        VkFramebuffer getFramebuffer(unsigned int index) const
        {
            if(swapChainFramebuffers.size()>index)
            {
                return swapChainFramebuffers[index];
            }
            else
            {
                VE_CORE_ERROR("VulkanSwapChain.getFramebuffer index out of bounds");
                return nullptr;
            }
        }

        void recreateSwapChain(const VulkanDevice& device, VkSurfaceKHR surface, VkRenderPass renderPass);

    private:
        void createSwapChain(const VulkanDevice& device, VkSurfaceKHR surface); 
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
        void cleanupSwapChain(VkDevice device);
        void createImageViews(const VkDevice device);
        void createFramebuffers(const VkDevice device, VkRenderPass renderPass);
        void createDepthResources(const VulkanDevice &device);

    private:
        VkSwapchainKHR swapChain;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
    };
} // namespace Core