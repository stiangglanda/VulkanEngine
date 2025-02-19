#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Renderer/Model.h"
#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>

namespace Core 
{
struct position 
{
    glm::vec3 value;
};

struct staticModel 
{
    std::shared_ptr<Core::Model> model;
};

class SceneGraph {
public:
    SceneGraph();
    ~SceneGraph();

    void update();
    entt::registry& getRegistry() { return registry; }

private:
    entt::registry registry;
};
}