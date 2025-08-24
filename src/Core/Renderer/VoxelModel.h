#pragma once
#include "../vepch.h"
#include "Vertex.h"
#include <vector>

namespace Core
{

constexpr uint32_t CHUNK_SIZE = 32;

struct VoxelChunk
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint8_t> voxels;
    glm::uvec3 chunkGridPosition;
    bool isDirty = true;
    bool isEmpty = true;

    VoxelChunk() : voxels(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, 0) {}

    [[nodiscard]] uint8_t getVoxel(uint32_t x, uint32_t y, uint32_t z) const;
    void setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex);
    void generateMesh();
};

class VoxelModel
{
public:
    explicit VoxelModel(const std::string& model_path);
    ~VoxelModel() = default;

    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    [[nodiscard]] glm::mat4 getModelMatrix() const { return modelMatrix; }
    [[nodiscard]] const std::vector<VoxelChunk>& getChunks() const { return chunks; }
    [[nodiscard]] const std::vector<glm::vec4>& getPalette() const { return palette; }

private:
    void loadFromVoxFile(const std::string& model_path);
    void setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex);

    glm::mat4 modelMatrix;
    glm::uvec3 modelDimensions;
    glm::uvec3 chunkGridDimensions;
    std::vector<VoxelChunk> chunks;
    std::vector<glm::vec4> palette;
};

} // namespace Core