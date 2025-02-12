#pragma once
#include "../../../vepch.h"
#include "../../Model.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "camera.h"

namespace Core
{
class VulkanModel : public Model 
{
public:
    VulkanModel(const std::string model_path, const std::string texture_path, VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command, const int max_frames_in_flight);
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

    void cleanup();

    void createVertexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createIndexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createUniformBuffers(VulkanDevice &device, const int max_frames_in_flight);
};
}