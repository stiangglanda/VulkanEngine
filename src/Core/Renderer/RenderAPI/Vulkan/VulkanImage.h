#pragma once
#include "../../../vepch.h"
#include "VulkanDevice.h"
#include <vulkan/vulkan.h>

namespace Core
{
// TODO look up how to restructure VulkanImage
class VulkanImage
{
  public:
    VulkanImage(VulkanDevice &device) : device(device)
    {
    }
    ~VulkanImage()
    {
    }

    void Init();
    void Shutdown();

    VkImage getImage() const
    {
        return textureImage;
    }

    VkImageView getImageView() const
    {
        return textureImageView;
    }

    VkSampler getSampler() const
    {
        return textureSampler;
    }

    void createTextureImage(const std::string TEXTURE_PATH, VkCommandPool commandPool,
                            VkQueue graphicsQueue);
    void createTextureImageView();
    void createTextureSampler();
    static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                            VkDeviceMemory &imageMemory, const VulkanDevice &device);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                            VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                            VkDeviceMemory &imageMemory);

  private:
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                           VkCommandPool commandPool, VkQueue graphicsQueue);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, 
                               VkCommandPool commandPool, VkQueue graphicsQueue);



    //TODO remove these functions
    //static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);//TODO find flace for this function
    VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool); // TODO should be in VulkanCommand
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool,
                               VkQueue graphicsQueue); // TODO should be in VulkanCommand
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, // TODO should be in VulkanBuffer
                      VkBuffer &buffer, VkDeviceMemory &bufferMemory);

  private:
    const VulkanDevice &device;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;//TODO this should not be here
};
} // namespace Core
