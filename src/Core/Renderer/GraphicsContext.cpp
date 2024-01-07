#include "GraphicsContext.h"
#include "../vepch.h"

#include "RenderAPI.h"
#include "RenderAPI/Vulkan/VulkanContext.h"

namespace Core
{

std::unique_ptr<GraphicsContext> GraphicsContext::Create(void *window)
{
    switch (RenderAPI::GetAPI())
    {
    case RenderAPI::API::None:
        VE_CORE_ERROR("RenderAPI::None is currently not supported!");
        return nullptr;
    case RenderAPI::API::Vulkan:
        return std::make_unique<VulkanContext>(static_cast<GLFWwindow *>(window));
    }
    VE_CORE_ERROR("Unknown RendererAPI!");
    return nullptr;
}

} // namespace Hazel
