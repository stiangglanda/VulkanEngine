#pragma once
#include "../../../vepch.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.h"
#include <vulkan/vulkan.h>

namespace Core
{
class VulkanImage;
// TODO look up how to restructure VulkanImage
struct ImageBuilder : public VulkanMemoryManager::Builder<ImageBuilder, VkImageCreateInfo>
{
  private:
    using Parent = VulkanMemoryManager::Builder<ImageBuilder, VkImageCreateInfo>;

  public:
    std::string texture_path;
    std::weak_ptr<VulkanCommandBuffer> command;

    ImageBuilder(VkExtent3D const &extent) : Parent(VkImageCreateInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr})
    {
        create_info.extent = extent;
        create_info.arrayLayers = 1;
        create_info.mipLevels = 1;
        create_info.imageType = VK_IMAGE_TYPE_2D;
        create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    }

    ImageBuilder(uint32_t width, uint32_t height = 1, uint32_t depth = 1)
        : ImageBuilder(VkExtent3D{width, height, depth})
    {
    }

    ImageBuilder &with_format(VkFormat format)
    {
        create_info.format = format;
        return *this;
    }

    ImageBuilder &with_usage(VkImageUsageFlags usage)
    {
        create_info.usage = usage;
        return *this;
    }

    ImageBuilder &with_sharing_mode(VkSharingMode sharing_mode)
    {
        create_info.sharingMode = sharing_mode;
        return *this;
    }

    ImageBuilder &with_flags(VkImageCreateFlags flags)
    {
        create_info.flags = flags;
        return *this;
    }

    ImageBuilder &with_image_type(VkImageType type)
    {
        create_info.imageType = type;
        return *this;
    }

    ImageBuilder &with_array_layers(uint32_t layers)
    {
        create_info.arrayLayers = layers;
        return *this;
    }

    ImageBuilder &with_mip_levels(uint32_t levels)
    {
        create_info.mipLevels = levels;
        return *this;
    }

    ImageBuilder &with_sample_count(VkSampleCountFlagBits sample_count)
    {
        create_info.samples = sample_count;
        return *this;
    }

    ImageBuilder &with_tiling(VkImageTiling tiling)
    {
        create_info.tiling = tiling;
        return *this;
    }

    ImageBuilder &with_implicit_sharing_mode()
    {
        if (create_info.queueFamilyIndexCount != 0)
        {
            create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
        }
        return *this;
    }

    template <typename ExtensionType> ImageBuilder &with_extension(ExtensionType &extension)
    {
        extension.pNext = create_info.pNext;

        create_info.pNext = &extension;

        return *this;
    }

    ImageBuilder &with_texture(const std::string TEXTURE_PATH, std::weak_ptr<VulkanCommandBuffer> Command)
    {
        texture_path = TEXTURE_PATH;
        command = Command;
        return *this;
    }

    VulkanImage build(VulkanDevice &device) const;
    std::unique_ptr<VulkanImage> build_unique(VulkanDevice &device) const;
};
class VulkanImage : public VulkanMemoryManager::VulkanMemoryManager<VkImage>
{
  public:
    VulkanImage(VulkanDevice &device, VkImage handle, const VkExtent3D &extent, VkFormat format,
                VkImageUsageFlags image_usage, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);

    // [[deprecated("Use the ImageBuilder ctor instead")]]
    VulkanImage(VulkanDevice &device, const VkExtent3D &extent, VkFormat format, VkImageUsageFlags image_usage,
                const std::string TEXTURE_PATH, std::weak_ptr<VulkanCommandBuffer> command,
                VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
                VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, uint32_t mip_levels = 1,
                uint32_t array_layers = 1, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkImageCreateFlags flags = 0,
                uint32_t num_queue_families = 0, const uint32_t *queue_families = nullptr);

    VulkanImage(VulkanDevice &device, ImageBuilder const &builder);

    VulkanImage(const VulkanImage &) = delete;

    VulkanImage(VulkanImage &&other) noexcept;

    ~VulkanImage();

    VulkanImage &operator=(const VulkanImage &) = delete;

    VulkanImage &operator=(VulkanImage &&) = delete;

    VkImageType get_type() const;

    const VkExtent3D &get_extent() const;

    VkFormat get_format() const;

    VkSampleCountFlagBits get_sample_count() const;

    VkImageUsageFlags get_usage() const;

    VkImageTiling get_tiling() const;

    const VkImageSubresource &get_subresource() const;

    uint32_t get_array_layer_count() const;

    // std::unordered_set<ImageView *> &get_views();

    VkDeviceSize get_image_required_size() const;

    // VkImageCompressionPropertiesEXT get_applied_compression() const;

    // TODO imageView and sampler shouldn't be here
    VkImageView getImageView() const
    {
        return view;
    }

    VkSampler getSampler() const
    {
        if (textureSampler != nullptr)
        {
            return textureSampler;
        }
        else
        {
            VE_CORE_ERROR("VulkanImage.getSampler tryed to access not initailised sampler");
            throw std::runtime_error("VulkanImage.getSampler tryed to access not initailised sampler");
            return nullptr;
        }
    }

    void createTextureImageView();
    void createTextureSampler();

    static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                       VkDevice device); // TODO should be in VulkanImageView

  private:
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                           std::weak_ptr<VulkanCommandBuffer> command);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                               std::weak_ptr<VulkanCommandBuffer> command);

  private:
    /// Image views referring to this image
    VkImageCreateInfo create_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    VkImageSubresource subresource{};
    VkImageView view;
    VkSampler textureSampler = nullptr; // TODO this should not be here
    // std::unordered_set<ImageView *> views;
};
} // namespace Core
