#pragma once
#include "../../../vepch.h"
#include "../../Model.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
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
    }
    ~VulkanModel() { cleanup(); }

    VkBuffer get_VertexBuffer_handle() const { return vertexBuffer->get_handle(); }
    VkBuffer get_IndexBuffer_handle() const { return indexBuffer->get_handle(); }

    // void updateUniforms(uint32_t currentFrame) override {
    //     // Vulkan-specific uniform buffer update
    //     // api.updateUniformBuffer(uniformBuffer, &modelMatrix, sizeof(modelMatrix));
    // }

    // void recordDrawCommands(CommandBuffer& commandBuffer) override {
    //     // Record Vulkan-specific draw commands
    //     // api.recordDrawCommands(commandBuffer, vertexBuffer, indexBuffer, descriptorSet);
    // }

private:
    // const VulkanDevice &device;
    // const std::weak_ptr<VulkanCommandBuffer> command;

    std::unique_ptr<VulkanBuffer> vertexBuffer;
    std::unique_ptr<VulkanBuffer> indexBuffer;
    // std::unique_ptr<VulkanDescriptorSet> descriptorSet;//TODO should probobly be in here

    void cleanup() {
        indexBuffer.reset();//TODO remove this is useless
        vertexBuffer.reset();//TODO remove this is useless
    }

    void createVertexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createIndexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    // void createUniformBuffers(VulkanDevice &device);//TODO should in the future be in VulkanModel
};
}