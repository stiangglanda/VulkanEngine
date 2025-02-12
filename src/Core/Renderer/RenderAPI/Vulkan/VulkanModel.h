#pragma once
#include "../../../vepch.h"
#include "../../Model.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "camera.h"


const std::string TEXTURE_PATH = RESOURCES_PATH "viking_room.png";

namespace Core
{
class VulkanModel : public Model 
{
public:
    VulkanModel(const std::string model_path, VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command, const int max_frames_in_flight) : Model(model_path) 
    {
        texture = ImageBuilder(0, 0) // with the option with_texture this will be set later
                  .with_format(VK_FORMAT_R8G8B8A8_SRGB)
                  .with_image_type(VK_IMAGE_TYPE_2D)
                  .with_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                  .with_tiling(VK_IMAGE_TILING_OPTIMAL)
                  .with_vma_usage(VMA_MEMORY_USAGE_AUTO)
                  .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                  .with_texture(TEXTURE_PATH, command)
                  .build_shared(device);

        texture->createTextureImageView();
        texture->createTextureSampler();

        createVertexBuffer(device, command);
        createIndexBuffer(device, command);
        createUniformBuffers(device, max_frames_in_flight);
        VE_CORE_INFO("Created VulkanModel");
    }
    ~VulkanModel() { cleanup(); }

    VkBuffer get_VertexBuffer_handle() const { return vertexBuffer->get_handle(); }
    VkBuffer get_IndexBuffer_handle() const { return indexBuffer->get_handle(); }
    const std::vector<std::unique_ptr<VulkanBuffer>>& get_uniformBuffers() const { return uniformBuffers; }
    const std::unique_ptr<VulkanBuffer>& get_uniformBuffersAt(unsigned int index) const { return uniformBuffers[index]; }
    const std::shared_ptr<VulkanImage> get_texture() const { return texture; }
    void updateUniformBuffer(uint32_t currentImage, Camera& Cam, float aspect);

private:
    std::unique_ptr<VulkanBuffer> vertexBuffer;
    std::unique_ptr<VulkanBuffer> indexBuffer;
    std::vector<std::unique_ptr<VulkanBuffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
    std::shared_ptr<VulkanImage> texture;

    void cleanup() 
    {
        texture.reset();

        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            uniformBuffers[i].reset(); // This is to avoid a segmentation fault since the memory
                                       // already gets freed by VulkanMemoryManager::shutdown and
                                       // then the destructore tryes to do the same therefore we call the destructure before
        }
        VE_CORE_INFO("Destroyed VulkanModel");
    }

    void createVertexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createIndexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createUniformBuffers(VulkanDevice &device, const int max_frames_in_flight);
};
}