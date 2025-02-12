#include "Renderer.h"
#include <memory>

namespace Core
{

std::unique_ptr<RenderAPI> Renderer::s_RenderAPI = RenderAPI::Create();

void Renderer::Init()
{
    s_RenderAPI->Init();
}

void Renderer::Shutdown()
{
    s_RenderAPI->Shutdown();
}

void Renderer::OnEvent(Event &e, float delta)
{
    s_RenderAPI->OnEvent(e, delta);
}

void Renderer::Draw()
{
    s_RenderAPI->Draw();
}

void Renderer::Update(float delta)
{
    s_RenderAPI->Update(delta);
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
}

std::unique_ptr<Model> Renderer::createModel(const std::string model_path, const std::string texture_path)
{
    VE_CORE_INFO("Create Model");
    switch (RenderAPI::GetAPI())
    {
    case RenderAPI::API::Vulkan:
        VE_CORE_INFO("Create Vulkan Model");
        return s_RenderAPI->LoadModel(model_path, texture_path);
        break;
    case RenderAPI::API::None:
        VE_CORE_ERROR("Can not create model with API None");
        return nullptr;
        break;
    default:
        VE_CORE_ERROR("Can not create model with unknown API");
        return nullptr;
        break;  
    }
}

void Renderer::BeginScene()
{
    // s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}
} // namespace Core
