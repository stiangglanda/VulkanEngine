#pragma once
#include "../vepch.h"

namespace Core
{

class RenderAPI
{
  public:
    enum class API
    {
        none = 0,
        Vulkan = 1
    };

    virtual ~RenderAPI() = default;
    virtual bool Init() = 0;
    virtual bool Shutdown() = 0;

    static API GetAPI()
    {
        return s_graphicsAPI;
    }
    static std::unique_ptr<RenderAPI> Create();

  private:
    static API s_graphicsAPI;
};
} // namespace Core
