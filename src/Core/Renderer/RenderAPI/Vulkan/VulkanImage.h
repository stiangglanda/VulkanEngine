#pragma once 
#include "VulkanDevice.h"
#include <vulkan/vulkan.h>
#include "../../../vepch.h"

namespace Core 
{
    class VulkanImage 
    {
    public:
        VulkanImage() {}
        ~VulkanImage() {}
        
        void Init(VkInstance instance, const bool EnableValidationLayers);
        void Shutdown(VkInstance instance);
    private:
        void createTextureImage(const std::string TEXTURE_PATH);
        void createTextureImageView();
        void createTextureSampler();
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                            VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                            VkDeviceMemory &imageMemory);
    private:
        const VulkanDevice& device;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
    };
} // namespace Core