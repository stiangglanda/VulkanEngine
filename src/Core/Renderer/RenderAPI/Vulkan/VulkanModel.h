#pragma once
#include "../../../vepch.h"
#include "../../Model.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "camera.h"

namespace Core
{
class VulkanModel : public Model 
{
public:
    VulkanModel(const std::string model_path, VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command, const int max_frames_in_flight) : Model(model_path) 
    {
        createVertexBuffer(device, command);
        createIndexBuffer(device, command);
        createUniformBuffers(device, max_frames_in_flight);
        VE_CORE_INFO("Created VulkanModel");
    }
    ~VulkanModel() { cleanup(); }

    VkBuffer get_VertexBuffer_handle() const { return vertexBuffer->get_handle(); }
    VkBuffer get_IndexBuffer_handle() const { return indexBuffer->get_handle(); }
    const std::vector<std::unique_ptr<VulkanBuffer>>& get_uniformBuffers() const { return uniformBuffers; }
    void updateUniformBuffer(uint32_t currentImage, Camera& Cam, float aspect);

private:
    std::unique_ptr<VulkanBuffer> vertexBuffer;
    std::unique_ptr<VulkanBuffer> indexBuffer;
    std::vector<std::unique_ptr<VulkanBuffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;

    void cleanup() 
    {
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