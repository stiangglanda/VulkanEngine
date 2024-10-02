#pragma once
#include "../../../vepch.h"
#include "../../Model.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"

namespace Core
{
class VulkanModel : public Model 
{
public:
    VulkanModel(const std::string model_path, VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command) : Model(model_path) 
    {
        createVertexBuffer(device, command);
        createIndexBuffer(device, command);
        VE_CORE_INFO("Created VulkanModel");
    }
    ~VulkanModel() { cleanup(); }

    VkBuffer get_VertexBuffer_handle() const { return vertexBuffer->get_handle(); }
    VkBuffer get_IndexBuffer_handle() const { return indexBuffer->get_handle(); }

private:
    std::unique_ptr<VulkanBuffer> vertexBuffer;
    std::unique_ptr<VulkanBuffer> indexBuffer;

    void cleanup() 
    {
        VE_CORE_INFO("Destroyed VulkanModel");
    }

    void createVertexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createIndexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
};
}