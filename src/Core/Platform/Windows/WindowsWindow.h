#pragma once
#include "../../Window.h"

#include <functional>
#include <GLFW/glfw3.h>
#include "../../Events/Event.h"

namespace Core
{

class WindowsWindow : public Window
{
  public:
    WindowsWindow(const WindowProps &props);
    virtual ~WindowsWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override
    {
        return m_Data.Width;
    }
    unsigned int GetHeight() const override
    {
        return m_Data.Height;
    }

    // Window attributes
    void SetEventCallback(const std::function<void(Event &)> &callback) override
    {
        m_Data.EventCallback = callback;
    }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    virtual void *GetNativeWindow() const
    {
        return m_Window;
    }

  private:
    virtual void Init(const WindowProps &props);
    virtual void Shutdown();

  private:
    GLFWwindow *m_Window;
    //Scope<GraphicsContext> m_Context;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        std::function<void(Event &)> EventCallback;
    };

    WindowData m_Data;
};

} // namespace Hazel