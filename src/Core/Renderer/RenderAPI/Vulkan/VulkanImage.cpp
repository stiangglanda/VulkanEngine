#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../vendor/stb_image.h"

namespace Core
{
inline VkImageType find_image_type(VkExtent3D extent)
{
    VkImageType result{};

    uint32_t dim_num{0};

    if (extent.width >= 1)
    {
        dim_num++;
    }

    if (extent.height >= 1)
    {
        dim_num++;
    }

    if (extent.depth > 1)
    {
        dim_num++;
    }

    switch (dim_num)
    {
    case 1:
        result = VK_IMAGE_TYPE_1D;
        break;
    case 2:
        result = VK_IMAGE_TYPE_2D;
        break;
    case 3:
        result = VK_IMAGE_TYPE_3D;
        break;
    default:
        throw std::runtime_error("No image type found.");
        break;
    }

    return result;
}

VulkanImage ImageBuilder::build(VulkanDevice &device) const
{
    return VulkanImage(device, *this);
}

std::unique_ptr<VulkanImage> ImageBuilder::build_unique(VulkanDevice &device) const
{
    return std::make_unique<VulkanImage>(device, *this);
}

std::shared_ptr<VulkanImage> ImageBuilder::build_shared(VulkanDevice &device) const
{
    return std::make_shared<VulkanImage>(device, *this);
}

VulkanImage::VulkanImage(VulkanDevice &device, const VkExtent3D &extent, VkFormat format, VkImageUsageFlags image_usage,
                         const std::string TEXTURE_PATH, std::weak_ptr<VulkanCommandBuffer> command,
                         VmaMemoryUsage memory_usage, VkSampleCountFlagBits sample_count, const uint32_t mip_levels,
                         const uint32_t array_layers, VkImageTiling tiling, VkImageCreateFlags flags,
                         uint32_t num_queue_families, const uint32_t *queue_families)
    : // Pass through to the ImageBuilder ctor
      VulkanImage(device, ImageBuilder(extent)
                              .with_format(format)
                              .with_image_type(find_image_type(extent))
                              .with_usage(image_usage)
                              .with_texture(TEXTURE_PATH, command)
                              .with_mip_levels(mip_levels)
                              .with_array_layers(array_layers)
                              .with_tiling(tiling)
                              .with_flags(flags)
                              .with_vma_usage(memory_usage)
                              .with_sample_count(sample_count)
                              .with_queue_families(num_queue_families, queue_families)
                              .with_implicit_sharing_mode())
{
}

VulkanImage::VulkanImage(VulkanDevice &device, ImageBuilder const &builder)
    : VulkanMemoryManager{builder.alloc_create_info, VK_NULL_HANDLE, &device}, create_info(builder.create_info)
{

    subresource.arrayLayer = create_info.arrayLayers;
    subresource.mipLevel = create_info.mipLevels;
    if (!builder.debug_name.empty())
    {
        // set_debug_name(builder.debug_name);
    }

    if (!builder.texture_path.empty())
    {
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(builder.texture_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(get_device(), imageSize, pixels);

        stbi_image_free(pixels);
        create_info.extent = VkExtent3D{static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1};
        set_handle(create_image(create_info));

        transitionImageLayout(get_handle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, builder.command);
        copyBufferToImage(stagingBuffer, get_handle(), static_cast<uint32_t>(texWidth),
                          static_cast<uint32_t>(texHeight), // Might not work try Handle
                          builder.command);
        transitionImageLayout(get_handle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, builder.command);
    }
    else
    {
        set_handle(create_image(create_info));
    }
    VE_CORE_INFO("VulkanImage created image");
}

VulkanImage::VulkanImage(VulkanDevice &device, VkImage handle, const VkExtent3D &extent, VkFormat format,
                         VkImageUsageFlags image_usage, VkSampleCountFlagBits sample_count)
    : VulkanMemoryManager{handle, &device}
{
    create_info.extent = extent;
    create_info.imageType = find_image_type(extent);
    create_info.format = format;
    create_info.samples = sample_count;
    create_info.usage = image_usage;
    subresource.arrayLayer = create_info.arrayLayers = 1;
    subresource.mipLevel = create_info.mipLevels = 1;
}

VulkanImage::VulkanImage(VulkanImage &&other) noexcept
    : VulkanMemoryManager{std::move(other)}, create_info{std::exchange(other.create_info, {})},
      subresource{std::exchange(other.subresource, {})}, view(std::exchange(other.view, {}))
{
    // Update image views references to this image to avoid dangling pointers
    // for (auto &view : views)
    // {
    // 	view->set_image(*this);
    // }
}

VulkanImage::~VulkanImage()
{
    destroy_image(get_handle());
    vkDestroySampler(get_device().getDevice(), textureSampler, nullptr); // TODO the sampler should not be here
    vkDestroyImageView(get_device().getDevice(), view, nullptr);
    VE_CORE_INFO("VulkanImage destroyed image");
}

VkImageType VulkanImage::get_type() const
{
    return create_info.imageType;
}

const VkExtent3D &VulkanImage::get_extent() const
{
    return create_info.extent;
}

VkFormat VulkanImage::get_format() const
{
    return create_info.format;
}

VkSampleCountFlagBits VulkanImage::get_sample_count() const
{
    return create_info.samples;
}

VkImageUsageFlags VulkanImage::get_usage() const
{
    return create_info.usage;
}

VkImageTiling VulkanImage::get_tiling() const
{
    return create_info.tiling;
}

const VkImageSubresource &VulkanImage::get_subresource() const
{
    return subresource;
}

uint32_t VulkanImage::get_array_layer_count() const
{
    return create_info.arrayLayers;
}

// std::unordered_set<ImageView *> &VulkanImage::get_views()
// {
// 	return views;
// }

VkDeviceSize VulkanImage::get_image_required_size() const
{
    VkMemoryRequirements memory_requirements;

    vkGetImageMemoryRequirements(get_device().getDevice(), get_handle(), &memory_requirements);

    return memory_requirements.size;
}

// VkImageCompressionPropertiesEXT VulkanImage::get_applied_compression() const
// {
// 	return query_applied_compression(get_device().getDevice(), get_handle());
// }

void VulkanImage::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                                        VkImageLayout newLayout, std::weak_ptr<VulkanCommandBuffer> command)
{
    VkCommandBuffer commandBuffer;

    if (auto tmp = command.lock())
    {
        commandBuffer = tmp->beginSingleTimeCommands();
    }
    else
    {
        VE_CORE_ERROR("VulkanImage::transitionImageLayout VulkanCommandBuffer ptr expired");
    }

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    if (auto tmp = command.lock())
    {
        tmp->endSingleTimeCommands(commandBuffer);
    }
    else
    {
        VE_CORE_ERROR("VulkanImage::transitionImageLayout VulkanCommandBuffer ptr expired");
    }
}

void VulkanImage::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                                    std::weak_ptr<VulkanCommandBuffer> command)
{
    VkCommandBuffer commandBuffer;

    if (auto tmp = command.lock())
    {
        commandBuffer = tmp->beginSingleTimeCommands();
    }
    else
    {
        VE_CORE_ERROR("VulkanImage::copyBufferToImage VulkanCommandBuffer ptr expired");
    }

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    if (auto tmp = command.lock())
    {
        tmp->endSingleTimeCommands(commandBuffer);
    }
    else
    {
        VE_CORE_ERROR("VulkanImage::copyBufferToImage VulkanCommandBuffer ptr expired");
    }
}

void VulkanImage::createTextureImageView()
{
    view = createImageView(get_handle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, get_device().getDevice());
}

void VulkanImage::createTextureSampler()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(get_device().getPhysicalDevice(), &properties);
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(get_device().getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkImageView VulkanImage::createImageView(VkImage image, VkFormat format,
                                         VkImageAspectFlags aspectFlags, // TODO should be in VulkanImageView
                                         VkDevice device)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}
} // namespace Core
