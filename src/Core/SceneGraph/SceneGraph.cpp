#include "SceneGraph.h"

namespace Core 
{

Node::Node(std::string name) : m_name(name) {}

void Node::addChild(std::shared_ptr<Node> child) {
    if (child) {
        child->setParent(shared_from_this());
        m_children.push_back(child);
    }
}

void Node::removeChild(std::shared_ptr<Node> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->setParent(nullptr);
        m_children.erase(it);
    }
}

void Node::setParent(std::shared_ptr<Node> parent) {
    m_parent = parent;
}

void Node::setLocalTransform(const glm::mat4& transform) {
    m_localTransform = transform;
}

glm::mat4 Node::getWorldTransform() const {
    glm::mat4 transform = m_localTransform;
    if (auto parent = m_parent.lock()) {
        transform = parent->getWorldTransform() * transform;
    }
    return transform;
}

void Node::setModel(std::shared_ptr<Model> model)
{
    m_model = std::move(model);
}

SceneGraph::SceneGraph() 
{
    m_root=std::make_shared<Node>("Root");
}

void SceneGraph::setRoot(std::shared_ptr<Node> root) {
    m_root = root;
}

SceneGraph::~SceneGraph() {
    VE_CORE_INFO("Shutdown SceneGraph");
    auto view = registry.view<staticModel>();

    for(auto [entity, model]: view.each()) {
        model.model.reset();
    }
}

void SceneGraph::update() {
    // Traverse the scene graph and update as needed
    // This is where you might update animations, physics, etc.
}

}