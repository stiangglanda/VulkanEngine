#include "VulkanModel.h"

namespace Core
{

void VulkanModel::createVertexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, bufferSize, vertices.data());

    vertexBuffer = (BufferBuilder(bufferSize)
                        .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
                        .with_vma_usage(VMA_MEMORY_USAGE_AUTO) // VMA_MEMORY_USAGE_GPU_ONLY
                        .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                        .build_unique(device));
    VulkanBuffer::copyBuffer(command, stagingBuffer.get_handle(), vertexBuffer->get_handle(), bufferSize);
}

void VulkanModel::createIndexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, bufferSize, indices.data());

    indexBuffer = (BufferBuilder(bufferSize)
                       .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                       .with_vma_usage(VMA_MEMORY_USAGE_AUTO) // VMA_MEMORY_USAGE_GPU_ONLY
                       .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                       .build_unique(device));
    VulkanBuffer::copyBuffer(command, stagingBuffer.get_handle(), indexBuffer->get_handle(), bufferSize);
}

// void VulkanModel::createUniformBuffers(VulkanDevice &device)
// {
//     VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//     uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//     uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//     {
//         uniformBuffers[i] = (BufferBuilder(bufferSize)
//                                  .with_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
//                                  .with_vma_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
//                                  .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
//                                  .build_unique(device));
//         uniformBuffersMapped[i] = uniformBuffers[i]->map();
//     }
// }

}