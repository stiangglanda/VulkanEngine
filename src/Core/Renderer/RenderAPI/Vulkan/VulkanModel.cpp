#include "VulkanModel.h"
#include "UniformBufferObject.h"

namespace Core
{

void VulkanModel::createVertexBuffer(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, bufferSize, vertices.data());

    vertexBuffer = (BufferBuilder(bufferSize)
                        .with_usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
                        .with_vma_usage(VMA_MEMORY_USAGE_GPU_ONLY) // VMA_MEMORY_USAGE_GPU_ONLY
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

void VulkanModel::createDescriptorSet(VulkanDevice &device, const int max_frames_in_flight, VkDescriptorSetLayout descriptorSetLayout)
{
    descriptorSet = std::make_unique<VulkanDescriptorSet>(
        device.getDevice(),
        max_frames_in_flight,
        descriptorSetLayout,
        uniformBuffers,
        vertexBuffer->get_handle(),
        vertices.size(),
        texture
    );
}

void VulkanModel::updateUniformBuffer(uint32_t currentImage, Camera& Cam, float aspect)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};

    ubo.view = Cam.getViewMatrix();
    ubo.model = getModelMatrix();
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);//TODO adjust far plane 
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

VulkanModel::VulkanModel(const std::string model_path, const std::string texture_path, VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command, const int max_frames_in_flight) : Model(model_path) 
{
    texture = ImageBuilder(0, 0)
              .with_format(VK_FORMAT_R8G8B8A8_SRGB)
              .with_image_type(VK_IMAGE_TYPE_2D)
              .with_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
              .with_tiling(VK_IMAGE_TILING_OPTIMAL)
              .with_vma_usage(VMA_MEMORY_USAGE_AUTO)
              .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
              .with_texture(texture_path, command)
              .build_shared(device);

    texture->createTextureImageView();
    texture->createTextureSampler();

    createVertexBuffer(device, command);
    createIndexBuffer(device, command);
    createUniformBuffers(device, max_frames_in_flight);
    VE_CORE_INFO("Created VulkanModel");
}

void VulkanModel::cleanup() 
{
    descriptorSet.reset();
    texture.reset();

    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        uniformBuffers[i].reset();
    }
    VE_CORE_INFO("Destroyed VulkanModel");
}

}