#include "RenderAPI.h"

namespace Core
{

RenderAPI::API RenderAPI::s_graphicsAPI = RenderAPI::API::Vulkan;

std::unique_ptr<RenderAPI> RenderAPI::Create()
{
    switch (s_graphicsAPI)
    {
    case API::Vulkan:
        VE_CORE_INFO("RenderAPI::Create Vulkan API");
        // return std::make_unique<VulkanAPI>();
        return nullptr;
        break;
    case API::none:
        VE_CORE_ERROR("RenderAPI::Create none API");
        return nullptr;
        break;
    }
}
} // namespace Core
