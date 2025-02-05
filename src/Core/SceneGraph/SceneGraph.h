#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Core 
{

class Node: public std::enable_shared_from_this<Node> {
public:
    Node(std::string name);
    ~Node() = default;

    void addChild(std::shared_ptr<Node> child);
    void removeChild(std::shared_ptr<Node> child);
    void setParent(std::shared_ptr<Node> parent);
    
    virtual void setLocalTransform(const glm::mat4& transform);
    virtual glm::mat4 getWorldTransform() const;
    
    std::string getName() const { return m_name; }
    std::vector<std::shared_ptr<Node>>& getChildren() { return m_children; }

private:
    std::string m_name;
    glm::mat4 m_localTransform{1.0f};
    std::weak_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children;
};

class SceneGraph {
public:
    SceneGraph();
    ~SceneGraph() = default;

    void setRoot(std::shared_ptr<Node> root);
    std::shared_ptr<Node> getRoot() { return m_root; }
    void update();

private:
    std::shared_ptr<Node> m_root;
};
}