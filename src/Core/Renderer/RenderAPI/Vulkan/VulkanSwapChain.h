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
        void createDepthResourcesAndFramebuffers(const VkDevice device, VkRenderPass renderPass, VkPhysicalDevice PhysicalDevice);
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

                return swapChainFramebuffers[index];

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
        void createDepthResources(const VkDevice device, VkPhysicalDevice PhysicalDevice);
        static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device);
        static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                            VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                            VkDeviceMemory &imageMemory, VkDevice device, VkPhysicalDevice PhysicalDevice);
        static VkFormat findDepthFormat(VkPhysicalDevice device);
        static VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                        VkFormatFeatureFlags features, VkPhysicalDevice device);
        static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device);

    private:
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
    };
} // namespace Core