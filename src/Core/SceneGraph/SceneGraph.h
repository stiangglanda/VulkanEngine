#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Node {
public:
    explicit Node(const std::string& name = "Node");
    ~Node() = default;
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;

    void addChild(std::shared_ptr<Node> child);
    void removeChild(std::shared_ptr<Node> child);
    void setParent(std::shared_ptr<Node> parent);
    
    virtual void setLocalTransform(const glm::mat4& transform);
    [[nodiscard]] virtual glm::mat4 getWorldTransform() const;
    
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
    SceneGraph(const SceneGraph&) = delete;
    SceneGraph& operator=(const SceneGraph&) = delete;
    SceneGraph(SceneGraph&&) = delete;
    SceneGraph& operator=(SceneGraph&&) = delete;

    void setRoot(std::shared_ptr<Node> root);
    [[nodiscard]] std::shared_ptr<Node> getRoot() const { return m_root; }
    void update();

private:
    std::shared_ptr<Node> m_root;
};
