#include "VulkanVoxelModel.h"
#include "UniformBufferObject.h"

namespace Core
{

VulkanVoxelModel::VulkanVoxelModel(const std::string model_path, VulkanDevice &device,
                                 const std::weak_ptr<VulkanCommandBuffer> command,
                                 const int max_frames_in_flight)
    : VoxelModel(model_path)
{
    createChunkBuffers(device, command);
    createUniformBuffers(device, max_frames_in_flight);
    VE_CORE_INFO("Created VulkanVoxelModel");
}

void VulkanVoxelModel::createChunkBuffers(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command)
{
    const auto& chunks = getChunks();
    chunkData.resize(chunks.size());

    for (size_t i = 0; i < chunks.size(); i++) {
        const auto& chunk = chunks[i];
        auto& vulkanChunk = chunkData[i];

        // Skip empty chunks
        if (chunk.isEmpty) {
            continue;
        }

        // Create vertex buffer
        VkDeviceSize vertexBufferSize = sizeof(Vertex) * chunk.vertices.size();
        if (vertexBufferSize > 0) {
            VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, vertexBufferSize, chunk.vertices.data());

            vulkanChunk.vertexBuffer = (BufferBuilder(vertexBufferSize)
                .with_usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
                .with_vma_usage(VMA_MEMORY_USAGE_GPU_ONLY)
                .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .build_unique(device));

            VulkanBuffer::copyBuffer(command, stagingBuffer.get_handle(), vulkanChunk.vertexBuffer->get_handle(), vertexBufferSize);
        }

        // Create index buffer
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * chunk.indices.size();
        if (indexBufferSize > 0) {
            VulkanBuffer stagingBuffer = VulkanBuffer::create_staging_buffer(device, indexBufferSize, chunk.indices.data());

            vulkanChunk.indexBuffer = (BufferBuilder(indexBufferSize)
                .with_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
                .with_vma_usage(VMA_MEMORY_USAGE_GPU_ONLY)
                .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .build_unique(device));

            VulkanBuffer::copyBuffer(command, stagingBuffer.get_handle(), vulkanChunk.indexBuffer->get_handle(), indexBufferSize);
        }

        vulkanChunk.indexCount = chunk.indices.size();
    }
}

void VulkanVoxelModel::createUniformBuffers(VulkanDevice &device, const int max_frames_in_flight)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    // Create uniform buffers for each chunk
    for (auto& chunk : chunkData) {
        chunk.uniformBuffers.resize(max_frames_in_flight);
        chunk.uniformBuffersMapped.resize(max_frames_in_flight);

        for (size_t i = 0; i < max_frames_in_flight; i++) {
            chunk.uniformBuffers[i] = (BufferBuilder(bufferSize)
                .with_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
                .with_vma_usage(VMA_MEMORY_USAGE_CPU_TO_GPU)
                .with_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
                .build_unique(device));
            chunk.uniformBuffersMapped[i] = chunk.uniformBuffers[i]->map();
        }
    }
}

void VulkanVoxelModel::createDescriptorSets(VulkanDevice &device, int max_frames_in_flight,
                                            VkDescriptorSetLayout descriptorSetLayout, std::weak_ptr<VulkanImage> texture)
{
    for (auto& chunk : chunkData) {
        if (!chunk.vertexBuffer || !chunk.indexBuffer) continue; // Skip empty chunks
        chunk.descriptorSet = std::make_unique<VulkanDescriptorSet>(
            device.getDevice(),
            max_frames_in_flight,
            descriptorSetLayout,
            chunk.uniformBuffers,
            chunk.vertexBuffer->get_handle(),
            chunk.vertexBuffer->get_size(),
            texture // No texture for voxel chunks yet
        );
    }
}

glm::mat4 VulkanVoxelModel::calculateChunkModelMatrix(const glm::uvec3& chunkGridPosition) const
{
    glm::vec3 chunkOffset = glm::vec3(chunkGridPosition);
    
    glm::mat4 chunkTranslation = glm::translate(getModelMatrix(), chunkOffset);
    
    return chunkTranslation;
}

void VulkanVoxelModel::updateUniformBuffers(uint32_t currentImage, Camera& cam, float aspect)
{
    const auto& chunks = getChunks();
    
    for (size_t i = 0; i < chunks.size(); i++) 
    {
        if (chunks[i].isEmpty) continue;

        UniformBufferObject ubo{};
        ubo.model = getModelMatrix();
        ubo.view = cam.getViewMatrix();
        ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        ubo.proj[1][1] *= -1; // Flip Y coordinate for Vulkan

        memcpy(chunkData[i].uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }
}

void VulkanVoxelModel::cleanup()
{
    for (auto& chunk : chunkData) {
        chunk.descriptorSet.reset();
        chunk.vertexBuffer.reset();
        chunk.indexBuffer.reset();
        for (auto& buffer : chunk.uniformBuffers) {
            buffer.reset();
        }
    }
    VE_CORE_INFO("Destroyed VulkanVoxelModel");
}

}
