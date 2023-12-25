#pragma once
#include "vepch.h"
#include "Events/Event.h"

namespace Core
{

struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;

    WindowProps(const std::string &title = "VulkanEngine", uint32_t width = 800, uint32_t height = 600)
        : Title(title), Width(width), Height(height)
    {
    }
};

// Interface representing a desktop system based Window
class Window
{
  public:

    virtual ~Window() = default;

    virtual void OnUpdate() = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    // Window attributes
    virtual void SetEventCallback(const std::function<void(Event &)> &callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    virtual void *GetNativeWindow() const = 0;

    static std::unique_ptr<Window> Create(const WindowProps &props = WindowProps());
};

} // namespace Core