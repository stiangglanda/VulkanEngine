#pragma once
#include "../vepch.h"
#include "Vertex.h"

namespace Core
{
class Model 
{
public:
    Model(const std::string model_path) : modelMatrix(glm::mat4(1.0f))
    {
        loadModel(model_path);
    };
    virtual ~Model() = default;

    // Common functionality
    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    [[nodiscard]] glm::mat4 getModelMatrix() const { return modelMatrix; }
    const size_t getverticesSize() const
    {
        return vertices.size();
    }
    const size_t getIndicesSize() const { return indices.size(); }

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