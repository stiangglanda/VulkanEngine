#include "Renderer.h"

namespace Core {

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

	void Renderer::BeginScene()
	{
		//s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}
}