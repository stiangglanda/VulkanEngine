#include "Renderer/Renderer.h"
#include <Application.h>
#include <Core.h>
#include <EntryPoint.h>
#include <SceneGraph/SceneGraph.h>

const std::string MODEL_PATH = RESOURCES_PATH "viking_room.obj";
const std::string TEXTURE_PATH = RESOURCES_PATH "viking_room.png";

class Sandbox : public Core::Application
{
  public:
    Sandbox(const Core::ApplicationSpecification &specification) : Core::Application(specification)
    {
      const auto entity = GetSceneGraph().getRegistry().create();
      GetSceneGraph().getRegistry().emplace<Core::position>(entity, glm::vec3(3.0f, 0.0f, 0.0f));
      GetSceneGraph().getRegistry().emplace<Core::staticModel>(entity, Core::Renderer::loadModel(MODEL_PATH, TEXTURE_PATH));
      GetSceneGraph().getRegistry().get<Core::staticModel>(entity).model->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));
    }

    ~Sandbox()
    {

    }
};

Core::Application *Core::CreateApplication(Core::ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "";
    spec.CommandLineArgs = args;

    return new Sandbox(spec);
}
