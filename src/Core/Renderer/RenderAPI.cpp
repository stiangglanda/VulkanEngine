#include "RenderAPI.h"

#include "RenderAPI/Vulkan/VulkanAPI.h"

namespace Core
{

RenderAPI::API RenderAPI::s_graphicsAPI = RenderAPI::API::Vulkan;

std::unique_ptr<RenderAPI> RenderAPI::Create()
{
    switch (s_graphicsAPI)
    {
    case API::Vulkan:
        VE_CORE_INFO("RenderAPI::Create Vulkan API");
        return std::make_unique<VulkanAPI>();
        break;
    case API::None:
        VE_CORE_ERROR("RenderAPI::Create None API");
        return nullptr;
        break;
    default:
        VE_CORE_ERROR("RenderAPI::Create API unknown");
        return nullptr;
        break;
    }
}
} // namespace Core
