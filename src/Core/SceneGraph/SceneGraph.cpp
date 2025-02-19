#include "SceneGraph.h"

namespace Core 
{

SceneGraph::SceneGraph() 
{
    VE_CORE_INFO("Init SceneGraph");
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