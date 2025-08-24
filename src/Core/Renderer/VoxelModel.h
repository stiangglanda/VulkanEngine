#pragma once
#include "../vepch.h"
#include "Vertex.h"
#include <vector>

namespace Core
{

// A constant defining the dimensions of a chunk (32x32x32 voxels)
constexpr uint32_t CHUNK_SIZE = 32;

// Represents a single, meshable chunk of the VoxelModel
struct VoxelChunk
{
    // The generated mesh data, ready for GPU upload
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // The raw voxel data for this chunk. Each uint8_t is an index into the model's palette.
    // A 1D vector is used for better memory locality. Access via [x + y*WIDTH + z*WIDTH*DEPTH]
    std::vector<uint8_t> voxels;

    // The position of this chunk within the VoxelModel's grid of chunks
    glm::uvec3 chunkGridPosition;

    bool isDirty = true; // Flag to indicate if the mesh needs to be regenerated
    bool isEmpty = true; // Optimization: if the chunk is all air, we can skip it

    VoxelChunk() : voxels(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, 0) {}

    // Helper to get a voxel's value at a local coordinate
    [[nodiscard]] uint8_t getVoxel(uint32_t x, uint32_t y, uint32_t z) const;

    // Helper to set a voxel's value and mark the chunk for remeshing
    void setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex);

    // Runs the Greedy Meshing algorithm to generate the vertices and indices
    void generateMesh();
};

// Represents a complete model loaded from a .vox file, composed of multiple chunks
class VoxelModel
{
public:
    explicit VoxelModel(const std::string& model_path);
    ~VoxelModel() = default;

    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    [[nodiscard]] glm::mat4 getModelMatrix() const { return modelMatrix; }

    // The renderer will use this to get all the individual chunk meshes to draw
    [[nodiscard]] const std::vector<VoxelChunk>& getChunks() const { return chunks; }
    [[nodiscard]] const std::vector<glm::vec4>& getPalette() const { return palette; }

private:
    void loadFromVoxFile(const std::string& model_path);
    void setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex);

    glm::mat4 modelMatrix;

    // Dimensions of the entire model in voxels
    glm::uvec3 modelDimensions;
    // Dimensions of the chunk grid
    glm::uvec3 chunkGridDimensions;

    // A flattened 1D vector of all chunks in the model
    std::vector<VoxelChunk> chunks;
    
    // The color palette loaded from the .vox file
    std::vector<glm::vec4> palette;
};

} // namespace Core