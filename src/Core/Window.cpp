#if defined(VE_PLATFORM_WINDOWS) || defined(VE_PLATFORM_LINUX)
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Core
{
std::unique_ptr<Window> Window::Create(const WindowProps &props)
{
#if defined(VE_PLATFORM_WINDOWS) || defined(VE_PLATFORM_LINUX)
    return std::make_unique<WindowsWindow>(props);
#else
    return nullptr;
#endif
}

} // namespace Core
