#include "MeshNode.h"

namespace Core {

MeshNode::MeshNode(const std::string& name)
    : Node(name)
{
}

MeshNode::MeshNode(const std::string& modelPath, const std::string& name)
    : Node(name)
{
    loadModel(modelPath);
}

void MeshNode::setModel(std::shared_ptr<Model> model)
{
    m_model = std::move(model);
}

void MeshNode::setLocalTransform(const glm::mat4& transform)
{
    Node::setLocalTransform(transform);
    if (m_model) {
        m_model->setModelMatrix(getWorldTransform());
    }
}

glm::mat4 MeshNode::getWorldTransform() const
{
    glm::mat4 worldTransform = Node::getWorldTransform();
    if (m_model) {
        m_model->setModelMatrix(worldTransform);
    }
    return worldTransform;
}

bool MeshNode::loadModel(const std::string& modelPath)
{
    try {
        m_model = std::make_shared<Model>(modelPath);
        return true;
    }
    catch (...) {
        return false;
    }
}

} // namespace Core
