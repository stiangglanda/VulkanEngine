#pragma once
#include "../Events/Event.h"
#include "../vepch.h"

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
    virtual void drawFrame() = 0;
    virtual void OnEvent(Event &e)
    {
        VE_CORE_INFO("Yea we are getting womewhere");
        // event hendling here
    }

    static API GetAPI()
    {
        return s_graphicsAPI;
    }
    static std::unique_ptr<RenderAPI> Create(void *window);

  private:
    static API s_graphicsAPI;
};
} // namespace Core
