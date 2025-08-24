#pragma once
#include "../../VoxelModel.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "camera.h"

namespace Core
{

// Structure to hold Vulkan-specific data for each chunk
struct VulkanChunkData {
    std::unique_ptr<VulkanBuffer> vertexBuffer;
    std::unique_ptr<VulkanBuffer> indexBuffer;
    std::vector<std::unique_ptr<VulkanBuffer>> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;
    uint32_t indexCount;
};

class VulkanVoxelModel : public VoxelModel 
{
public:
    VulkanVoxelModel(const std::string model_path, VulkanDevice &device,
                     const std::weak_ptr<VulkanCommandBuffer> command,
                     const int max_frames_in_flight);
    ~VulkanVoxelModel() { cleanup(); }

    const std::vector<VulkanChunkData>& getChunkData() const { return chunkData; }
    void updateUniformBuffers(uint32_t currentImage, Camera& cam, float aspect);

private:
    std::vector<VulkanChunkData> chunkData;
    
    void cleanup();
    void createChunkBuffers(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);
    void createUniformBuffers(VulkanDevice &device, const int max_frames_in_flight);
    glm::mat4 calculateChunkModelMatrix(const glm::uvec3& chunkGridPosition) const;
};

}
