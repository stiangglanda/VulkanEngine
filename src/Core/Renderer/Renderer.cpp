#include "Renderer.h"

namespace Core {

	std::unique_ptr<RenderAPI> Renderer::mRenderAPI = RenderAPI::Create();

	void Renderer::Init()
	{
        mRenderAPI->Init();
	}

	void Renderer::Shutdown()
	{
		mRenderAPI->Shutdown();
	}

    void Renderer::OnEvent(Event &e, float delta)
	{
        mRenderAPI->OnEvent(e, delta);
	}

    void Renderer::Draw()
	{
        mRenderAPI->Draw();
	}

    void Renderer::Update(float delta)
	{
        mRenderAPI->Update(delta);
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