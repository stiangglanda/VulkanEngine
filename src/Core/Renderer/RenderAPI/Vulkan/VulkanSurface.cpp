#include "VulkanSurface.h"
#include "../../../Application.h"
#include <GLFW/glfw3.h>

namespace Core
{

void VulkanSurface::Init(VkInstance instance)
{

#if defined(VE_PLATFORM_WINDOWS) || defined(VE_PLATFORM_LINUX)
    if (glfwCreateWindowSurface(instance, static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow()),
                                nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
    else
    {
        VE_CORE_INFO("Successful: Init Vulkan Surface");
    }
#else
    return nullptr;
#endif
}

void VulkanSurface::Shutdown(VkInstance instance)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
    VE_CORE_INFO("Shutdown Vulkan Surface");
}

} // namespace Core
