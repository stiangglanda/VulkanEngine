#pragma once
#include "../vepch.h"
#include "Model.h"
#include "RenderAPI.h"
#include "VoxelModel.h"

namespace Core
{

class Renderer
{
  public:
    static void Init();
    static void Shutdown();

    static void OnWindowResize(uint32_t width, uint32_t height);

    static void BeginScene(); // Camera& camera
    static void EndScene();
    static void OnEvent(Event &e, float delta);
    static void Draw();
    static void Update(float delta);
    static std::shared_ptr<Model> loadModel(const std::string model_path, const std::string texture_path);
    static std::shared_ptr<VoxelModel> loadVoxelModel(const std::string model_path);

    // static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform =
    // glm::mat4(1.0f));

    static RenderAPI::API GetAPI()
    {
        return RenderAPI::GetAPI();
    }

  private:
    static std::unique_ptr<RenderAPI> s_RenderAPI;
};
} // namespace Core
