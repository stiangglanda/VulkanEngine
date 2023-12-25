#ifdef VE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Core
{
std::unique_ptr<Window> Window::Create(const WindowProps &props)
{
#ifdef VE_PLATFORM_WINDOWS
    return std::make_unique<WindowsWindow>(props);
#else
    return nullptr;
#endif
}

} // namespace Core
