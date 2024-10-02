#pragma once
#include "../vepch.h"
#include "Vertex.h"
#include <cstddef>

namespace Core
{
class Model 
{
public:
    Model(const std::string model_path)
    {
        loadModel(model_path);
    };
    virtual ~Model() = default;

    // Common functionality
    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    [[nodiscard]] glm::mat4 getModelMatrix() const { return modelMatrix; }
    size_t getIndicesSize() { return indices.size(); }

    // API-specific interface
    // virtual void initialize() = 0;
    // virtual void updateUniforms(uint32_t currentFrame) = 0;
    // virtual void recordDrawCommands(CommandBuffer& commandBuffer) = 0;//TODO add a general CommandBuffer class

protected:
    void loadModel(const std::string model_path);

    glm::mat4 modelMatrix;
    //TODO add texture

    // Shared data like vertex data, indices, etc.
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
}