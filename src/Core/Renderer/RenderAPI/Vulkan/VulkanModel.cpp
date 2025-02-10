#include "VulkanModel.h"
#include "UniformBufferObject.h"

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

void VulkanModel::createUniformBuffers(VulkanDevice &device, const int max_frames_in_flight)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    uniformBuffers.resize(max_frames_in_flight);
    uniformBuffersMapped.resize(max_frames_in_flight);

    for (size_t i = 0; i < max_frames_in_flight; i++)
    {
        uniformBuffers[i] = (BufferBuilder(bufferSize)
                                 .with_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                                 .with_vma_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
                                 .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                                 .build_unique(device));
        uniformBuffersMapped[i] = uniformBuffers[i]->map();
    }
}

void VulkanModel::updateUniformBuffer(uint32_t currentImage, Camera& Cam, float aspect) // TODO use actual camera
{//TODO move everything ralatiing to camera to VulkanAPI into sepret UniformBuffer
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    // Cam.update();
    UniformBufferObject ubo{};

    ubo.view = Cam.getViewMatrix();

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = getModelMatrix();//TODO shoul be seperate from camera cause per object
    // ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect,
                                0.1f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

}