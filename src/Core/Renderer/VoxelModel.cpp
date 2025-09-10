#include "VoxelModel.h"
#include <fstream>
#include <vector>
#include <iostream>

#define OGT_VOX_IMPLEMENTATION
#include "../../vendor/ogt_vox.h"

namespace Core
{

uint8_t VoxelChunk::getVoxel(int x, int y, int z) const
{
    if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
    {
        return 0;
    }
    return voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
}

void VoxelChunk::setVoxel(uint32_t x, uint32_t y, uint32_t z, uint8_t paletteIndex)
{
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) return;

    voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = paletteIndex;
    isDirty = true;
    isEmpty = paletteIndex != 0;
}

void VoxelChunk::generateMesh(const std::vector<glm::vec4> &palette)
{
    if (!isDirty || isEmpty)
        return;

    vertices.clear();
    indices.clear();

    for (int axis = 0; axis < 3; ++axis)
    {
        for (int dir = -1; dir <= 1; dir += 2)
        {
            int u = (axis + 1) % 3;
            int v = (axis + 2) % 3;

            glm::ivec3 x = {0, 0, 0};
            glm::vec3 normal = {0, 0, 0};
            normal[axis] = static_cast<float>(dir);

            std::vector<uint8_t> mask(CHUNK_SIZE * CHUNK_SIZE, 0);

            for (x[axis] = -1; x[axis] < (int)CHUNK_SIZE; ++x[axis])
            {
                int n = 0;
                for (x[v] = 0; x[v] < (int)CHUNK_SIZE; ++x[v])
                {
                    for (x[u] = 0; x[u] < (int)CHUNK_SIZE; ++x[u])
                    {
                        uint8_t voxelCurrent = getVoxel(x[0], x[1], x[2]);
                        uint8_t voxelNeighbor = getVoxel(x[0] + normal.x, x[1] + normal.y, x[2] + normal.z);

                        if (voxelCurrent == voxelNeighbor)
                        {
                            mask[n++] = 0;
                        }
                        else if (voxelCurrent != 0 && voxelNeighbor == 0)
                        {
                            mask[n++] = voxelCurrent;
                        }
                        else if (voxelCurrent == 0 && voxelNeighbor != 0)
                        {
                            mask[n++] = voxelNeighbor;
                        }
                        else
                        {
                            mask[n++] = voxelCurrent;
                        }
                    }
                }

                n = 0;
                for (int j = 0; j < CHUNK_SIZE; j++)
                {
                    for (int i = 0; i < CHUNK_SIZE;)
                    {
                        if (mask[n] != 0)
                        {
                            uint8_t current_color_index = mask[n];

                            int w = 1;
                            while (i + w < CHUNK_SIZE && mask[n + w] == current_color_index)
                            {
                                w++;
                            }

                            int h = 1;
                            bool done = false;
                            while (j + h < CHUNK_SIZE)
                            {
                                for (int k = 0; k < w; k++)
                                {
                                    if (mask[n + k + h * CHUNK_SIZE] != current_color_index)
                                    {
                                        done = true;
                                        break;
                                    }
                                }
                                if (done)
                                    break;
                                h++;
                            }

                            x[u] = i;
                            x[v] = j;

                            glm::vec3 quad_pos = {(float)x[0], (float)x[1], (float)x[2]};
                            quad_pos[axis] += 1;

                            glm::vec3 du = {0, 0, 0};
                            du[u] = (float)w;
                            glm::vec3 dv = {0, 0, 0};
                            dv[v] = (float)h;

                            glm::vec3 chunkOffset = chunkGridPosition * CHUNK_SIZE;

                            Vertex v1, v2, v3, v4;
                            v1.position = chunkOffset + quad_pos;
                            v2.position = chunkOffset + quad_pos + du;
                            v3.position = chunkOffset + quad_pos + du + dv;
                            v4.position = chunkOffset + quad_pos + dv;

                            glm::vec4 color = palette[current_color_index];
                            v1.color = color;
                            v2.color = color;
                            v3.color = color;
                            v4.color = color;
                            v1.normal = normal;
                            v2.normal = normal;
                            v3.normal = normal;
                            v4.normal = normal;

                            v1.uv_x = 0.0f;
                            v1.uv_y = 0.0f;
                            v2.uv_x = (float)w;
                            v2.uv_y = 0.0f;
                            v3.uv_x = (float)w;
                            v3.uv_y = (float)h;
                            v4.uv_x = 0.0f;
                            v4.uv_y = (float)h;

                            uint32_t base_index = static_cast<uint32_t>(vertices.size());

                            if (dir > 0)
                            {
                                indices.push_back(base_index + 0);
                                indices.push_back(base_index + 1);
                                indices.push_back(base_index + 2);
                                indices.push_back(base_index + 0);
                                indices.push_back(base_index + 2);
                                indices.push_back(base_index + 3);
                            }
                            else
                            {
                                indices.push_back(base_index + 0);
                                indices.push_back(base_index + 2);
                                indices.push_back(base_index + 1);
                                indices.push_back(base_index + 0);
                                indices.push_back(base_index + 3);
                                indices.push_back(base_index + 2);
                            }
                            vertices.push_back(v1);
                            vertices.push_back(v2);
                            vertices.push_back(v3);
                            vertices.push_back(v4);

                            for (int l = 0; l < h; ++l)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    mask[n + k + l * CHUNK_SIZE] = 0;
                                }
                            }

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

    uint32_t chunkX = x / CHUNK_SIZE;
    uint32_t chunkY = y / CHUNK_SIZE;
    uint32_t chunkZ = z / CHUNK_SIZE;

    uint32_t localX = x % CHUNK_SIZE;
    uint32_t localY = y % CHUNK_SIZE;
    uint32_t localZ = z % CHUNK_SIZE;

    size_t chunkIndex = chunkX + chunkY * chunkGridDimensions.x + chunkZ * chunkGridDimensions.x * chunkGridDimensions.y;
    chunks[chunkIndex].setVoxel(localX, localY, localZ, paletteIndex);
}

void VoxelModel::loadFromVoxFile(const std::string& model_path)
{
    std::ifstream file(model_path, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Failed to open .vox file: " + model_path);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});

    const ogt_vox_scene* scene = ogt_vox_read_scene(buffer.data(), buffer.size());
    if (!scene) throw std::runtime_error("Failed to parse .vox file: " + model_path);

    if (scene->num_models > 0)
    {
        const ogt_vox_model* model = scene->models[0];
        modelDimensions = { model->size_x, model->size_y, model->size_z };

        VE_CORE_INFO("VoxelModel: {}x{}x{} (hash={})", model->size_x, model->size_y, model->size_z, model->voxel_hash);

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
        chunk.generateMesh(palette);
    }
}

} // namespace Core