#pragma once
#include "RenderAPI.h"
#include "../vepch.h"

namespace Core {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene();//Camera& camera
		static void EndScene();
		static void OnEvent(Event &e, float delta);
		static void Draw();
		static void Update(float delta);

		//static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
    private:
        static std::unique_ptr<RenderAPI> mRenderAPI;
	};
}