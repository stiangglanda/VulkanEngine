#pragma once
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

    static API GetAPI()
    {
        return s_graphicsAPI;
    }
    static std::unique_ptr<RenderAPI> Create();

  private:
    static API s_graphicsAPI;
};
} // namespace Core
