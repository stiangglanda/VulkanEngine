#pragma once
#include "../../VoxelModel.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanDescriptorSet.h"
#include "camera.h"

namespace Core
{

struct VulkanChunkData {
    std::unique_ptr<VulkanBuffer> vertexBuffer;
    std::unique_ptr<VulkanBuffer> indexBuffer;
    std::vector<std::unique_ptr<VulkanBuffer>>
        uniformBuffers; // TODO remove uniformBuffers in chunk because the VoxelModel only needs one UBO with the model matrix and not per chunk
    std::vector<void*> uniformBuffersMapped;
    uint32_t indexCount;
    std::unique_ptr<VulkanDescriptorSet> descriptorSet; // Descriptor set for this chunk
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
    void createDescriptorSets(VulkanDevice &device, int max_frames_in_flight, VkDescriptorSetLayout descriptorSetLayout,
                              std::weak_ptr<VulkanImage> texture);
    VkDescriptorSet* get_descriptorSet_handle_at_index(size_t chunkIdx, unsigned int frameIdx) {
        return chunkData[chunkIdx].descriptorSet->get_handle_ptr_at_index(frameIdx);
    }

private:
    std::vector<VulkanChunkData> chunkData;
    
    void cleanup();
    void createChunkBuffers(VulkanDevice &device, const std::weak_ptr<VulkanCommandBuffer> command);//TODO move uniformBuffers from VulkanChunkData to VulkanVoxelModel
    void createUniformBuffers(VulkanDevice &device, const int max_frames_in_flight);
    glm::mat4 calculateChunkModelMatrix(const glm::uvec3& chunkGridPosition) const;//TODO no longer needed
};

}
