#include "VoxelModel.h"
#include <fstream>
#include <vector>
#include <iostream>

// Place the implementation macro in this .cpp file
#define OGT_VOX_IMPLEMENTATION
#include "../../vendor/ogt_vox.h"

namespace Core
{

// --- VoxelChunk Implementation ---

uint8_t VoxelChunk::getVoxel(uint32_t x, uint32_t y, uint32_t z) const
{
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
    {
        return 0; // Out of bounds is treated as air
    }
    return voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void VoxelChunk::setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex)
{
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) return;

    voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = paletteIndex;
    isDirty = true;
    if (paletteIndex != 0)
    {
        isEmpty = false;
    }
}

void VoxelChunk::generateMesh()
{
    if (!isDirty || isEmpty) return;

    vertices.clear();
    indices.clear();

    // The core Greedy Meshing algorithm
    // This process is repeated 6 times for each face direction (positive/negative X, Y, Z)
    for (int axis = 0; axis < 3; ++axis)
    {
        for (int dir = -1; dir <= 1; dir += 2)
        {
            // Create a mask for the current slice
            std::vector<uint8_t> mask(CHUNK_SIZE * CHUNK_SIZE, 0);

            // Axis indices
            int u = (axis + 1) % 3;
            int v = (axis + 2) % 3;
            
            // World-space position vector
            glm::ivec3 x = {0, 0, 0};
            // Normal vector for this face direction
            glm::vec3 normal = {0, 0, 0};
            normal[axis] = dir;

            // Iterate through slices of the chunk
            for (x[axis] = -1; x[axis] < (int)CHUNK_SIZE; )
            {
                // --- 1. Create the mask ---
                int n = 0;
                for (x[v] = 0; x[v] < (int)CHUNK_SIZE; ++x[v])
                {
                    for (x[u] = 0; x[u] < (int)CHUNK_SIZE; ++x[u])
                    {
                        uint8_t voxelCurrent = getVoxel(x[0], x[1], x[2]);
                        uint8_t voxelNeighbor = getVoxel(x[0] + normal.x, x[1] + normal.y, x[2] + normal.z);

                        bool currentSolid = (voxelCurrent != 0);
                        bool neighborSolid = (voxelNeighbor != 0);

                        // A face is visible if a solid voxel is next to a non-solid one
                        if (currentSolid == neighborSolid) {
                            mask[n++] = 0;
                        } else if (currentSolid) {
                            mask[n++] = voxelCurrent;
                        } else {
                            mask[n++] = voxelNeighbor;
                        }
                    }
                }

                x[axis]++; // Move to the next slice

                // --- 2. Generate quads from the mask ---
                n = 0;
                for (int j = 0; j < CHUNK_SIZE; ++j)
                {
                    for (int i = 0; i < CHUNK_SIZE; )
                    {
                        if (mask[n] != 0)
                        {
                            uint8_t current_color_index = mask[n];

                            // Greedily find the width of the quad
                            int w = 1;
                            while (i + w < CHUNK_SIZE && mask[n + w] == current_color_index)
                            {
                                w++;
                            }

                            // Greedily find the height of the quad
                            int h = 1;
                            bool done = false;
                            while (j + h < CHUNK_SIZE)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    if (mask[n + k + h * CHUNK_SIZE] != current_color_index)
                                    {
                                        done = true;
                                        break;
                                    }
                                }
                                if (done) break;
                                h++;
                            }
                            
                            // --- 3. Add vertices for this quad ---
                            x[u] = i;
                            x[v] = j;

                            glm::vec3 du = {0,0,0}; du[u] = (float)w;
                            glm::vec3 dv = {0,0,0}; dv[v] = (float)h;

                            // Calculate world position offset for this chunk
                            glm::vec3 chunkOffset = chunkGridPosition * CHUNK_SIZE;
                            
                            // Create the 4 vertices of the quad
                            Vertex v1, v2, v3, v4;
                            v1.position = chunkOffset + glm::vec3(x[0], x[1], x[2]);
                            v2.position = chunkOffset + glm::vec3(x[0], x[1], x[2]) + du;
                            v3.position = chunkOffset + glm::vec3(x[0], x[1], x[2]) + du + dv;
                            v4.position = chunkOffset + glm::vec3(x[0], x[1], x[2]) + dv;

                            // Set UVs based on world position (for tiled texturing in shader)
                            v1.uv_x = v1.position.x; v1.uv_y = v1.position.y;
                            v2.uv_x = v2.position.x; v2.uv_y = v2.position.y;
                            v3.uv_x = v3.position.x; v3.uv_y = v3.position.y;
                            v4.uv_x = v4.position.x; v4.uv_y = v4.position.y;
                            
                            // A custom attribute for texture index can be added to Vertex struct
                            // v1.texture_index = current_color_index;
                            
                            v1.normal = normal; v2.normal = normal; v3.normal = normal; v4.normal = normal;

                            // Add indices (2 triangles)
                            uint32_t base_index = vertices.size();
                            if (dir > 0) { // Positive face direction
                                indices.push_back(base_index); indices.push_back(base_index + 2); indices.push_back(base_index + 1);
                                indices.push_back(base_index); indices.push_back(base_index + 3); indices.push_back(base_index + 2);
                            } else { // Negative face direction (reverse winding order)
                                indices.push_back(base_index); indices.push_back(base_index + 1); indices.push_back(base_index + 2);
                                indices.push_back(base_index); indices.push_back(base_index + 2); indices.push_back(base_index + 3);
                            }
                            vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3); vertices.push_back(v4);
                            

                            // --- 4. Zero out the mask for the area we just meshed ---
                            for (int l = 0; l < h; ++l)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    mask[n + k + l * CHUNK_SIZE] = 0;
                                }
                            }

                            // Increment counters
                            i += w;
                            n += w;
                        }
                        else
                        {
                            i++;
                            n++;
                        }
                    }
                }
            }
        }
    }
    isDirty = false;
}

// --- VoxelModel Implementation ---

VoxelModel::VoxelModel(const std::string& model_path) : modelMatrix(glm::mat4(1.0f))
{
    loadFromVoxFile(model_path);
}

void VoxelModel::setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex)
{
    if (x >= modelDimensions.x || y >= modelDimensions.y || z >= modelDimensions.z) return;

    // Determine which chunk this voxel belongs to
    uint32_t chunkX = x / CHUNK_SIZE;
    uint32_t chunkY = y / CHUNK_SIZE;
    uint32_t chunkZ = z / CHUNK_SIZE;

    // Calculate local coordinates within that chunk
    uint32_t localX = x % CHUNK_SIZE;
    uint32_t localY = y % CHUNK_SIZE;
    uint32_t localZ = z % CHUNK_SIZE;

    size_t chunkIndex = chunkX + chunkY * chunkGridDimensions.x + chunkZ * chunkGridDimensions.x * chunkGridDimensions.y;
    chunks[chunkIndex].setVoxel(localX, localY, localZ, paletteIndex);
}

void VoxelModel::loadFromVoxFile(const std::string& model_path)
{
    // 1. Read file into a buffer
    std::ifstream file(model_path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to open .vox file: " + model_path);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});

    // 2. Parse buffer with ogt_vox
    const ogt_vox_scene* scene = ogt_vox_read_scene(buffer.data(), buffer.size());
    if (!scene) throw std::runtime_error("Failed to parse .vox file: " + model_path);

    // 3. Load the first model in the scene
    if (scene->num_models > 0)
    {
        const ogt_vox_model* model = scene->models[0];
        modelDimensions = { model->size_x, model->size_y, model->size_z };

        // 4. Calculate chunk grid dimensions and initialize chunks
        chunkGridDimensions = (modelDimensions + (CHUNK_SIZE - 1u)) / CHUNK_SIZE;
        chunks.resize(chunkGridDimensions.x * chunkGridDimensions.y * chunkGridDimensions.z);

        for (uint32_t z = 0; z < chunkGridDimensions.z; ++z) {
            for (uint32_t y = 0; y < chunkGridDimensions.y; ++y) {
                for (uint32_t x = 0; x < chunkGridDimensions.x; ++x) {
                    size_t index = x + y * chunkGridDimensions.x + z * chunkGridDimensions.x * chunkGridDimensions.y;
                    chunks[index].chunkGridPosition = {x, y, z};
                }
            }
        }

        // 5. Populate our chunk data from the sparse voxel list
        //for (uint32_t i = 0; i < model->size_x * model->size_y * model->size_z; ++i)
        //{
            // MagicaVoxel Y is up, but many engines use Z up. Here we assume Y is up.
        //    setVoxel(v.x, v.y, v.z, model->voxel_data[i]);
        //}

        for (uint32_t x = 0; x < model->size_x; x++)
        {
            for (uint32_t y = 0; y < model->size_y; y++)
            {
                for (uint32_t z = 0; z < model->size_z; z++)
                {
                    setVoxel(x, y, z, model->voxel_data[x + y * model->size_x + z * model->size_x * model->size_y]);
                }
            }
        }

        // 6. Load the color palette
        palette.resize(256);
        for (int i = 0; i < 256; ++i)
        {
            ogt_vox_rgba color = scene->palette.color[i];
            palette[i] = glm::vec4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
        }
    }

    // 7. Clean up
    ogt_vox_destroy_scene(scene);

    // 8. Generate the initial mesh for every chunk
    // This can be parallelized for faster loading!
    for (auto& chunk : chunks)
    {
        chunk.generateMesh();
    }
}

} // namespace Core