#include "Renderer/Renderer.h"
#include "Renderer/VoxelModel.h"
#include <Application.h>
#include <Core.h>
#include <EntryPoint.h>
#include <SceneGraph/SceneGraph.h>
#include <memory>

const std::string MODEL_PATH = RESOURCES_PATH "viking_room.obj";
const std::string TEXTURE_PATH = RESOURCES_PATH "viking_room.png";
const std::string VOXEL_MODEL_PATH = RESOURCES_PATH "house.vox";

class Sandbox : public Core::Application
{
  public:
    Sandbox(const Core::ApplicationSpecification &specification) : Core::Application(specification)
    {
      using namespace Core;
      const auto entity = GetSceneGraph().getRegistry().create();
      GetSceneGraph().getRegistry().emplace<position>(entity, glm::vec3(3.0f, 0.0f, 0.0f));
      GetSceneGraph().getRegistry().emplace<staticModel>(entity, Renderer::loadModel(MODEL_PATH, TEXTURE_PATH));
      GetSceneGraph().getRegistry().get<staticModel>(entity).model->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));


      const auto entity2 = GetSceneGraph().getRegistry().create();
      GetSceneGraph().getRegistry().emplace<Core::position>(entity2, glm::vec3(3.0f, 0.0f, 0.0f));
      GetSceneGraph().getRegistry().emplace<voxelModel>(entity2, Core::Renderer::loadVoxelModel(VOXEL_MODEL_PATH));
      GetSceneGraph().getRegistry().get<voxelModel>(entity2).model->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));
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
