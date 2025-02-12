#pragma once
#include "../Events/Event.h"
#include "../vepch.h"
#include "Model.h"

namespace Core
{

class RenderAPI
{
  public:
    enum class API
    {
        None = 0,
        Vulkan = 1
    };

    virtual ~RenderAPI() = default;

    virtual bool Init() = 0;
    virtual bool Shutdown() = 0;
    virtual void Update(float delta) = 0;
    virtual void Draw() = 0;
    virtual void OnEvent(Event &e, float delta)
    {
        VE_CORE_INFO("Yea we are getting somewhere");
        // event hendling here
    }
    virtual std::unique_ptr<Model> LoadModel(const std::string model_path,const std::string texture_path)=0;


    static API GetAPI()
    {
        return s_graphicsAPI;
    }
    static std::unique_ptr<RenderAPI> Create();

  private:
    static API s_graphicsAPI;
};
} // namespace Core
