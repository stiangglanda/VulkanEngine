#include "MeshNode.h"

namespace Core {

MeshNode::MeshNode(const std::string& name)
    : Node(name)
{
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
