#pragma once
#include "SceneGraph.h"
#include "../Renderer/Model.h"

namespace Core 
{

class MeshNode : public Node 
{
public:
    MeshNode(const std::string& name = "MeshNode");
    virtual ~MeshNode() = default;

    void setModel(std::shared_ptr<Model> model);
    [[nodiscard]] std::shared_ptr<Model> getModel() const { return m_model; }
    
    // Override transform methods to update model matrix
    void setLocalTransform(const glm::mat4& transform) override;
    [[nodiscard]] glm::mat4 getWorldTransform() const override;

    // Helper method to load a model directly
    bool loadModel(const std::string& modelPath);

private:
    std::shared_ptr<Model> m_model;
};

} // namespace Core
