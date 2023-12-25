#include "Application.h"

namespace Core
{

Application *Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification &specification) : m_Specification(specification)
{
    s_Instance = this;
    // if (!m_Specification.WorkingDirectory.empty())
    //     std::filesystem::current_path(m_Specification.WorkingDirectory);

    mWindow = Window::Create(WindowProps(m_Specification.Name));
    mWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

Application::~Application()
{
    Shutdown();
}

void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
    dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

    // event hendling here
}

bool Application::OnWindowClose(WindowCloseEvent &e)
{
    mRunning = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent &e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        mMinimized = true;
        return false;
    }

    mMinimized = false;

    return true;
}

void Application::Shutdown()
{
}

void Application::Run()
{

    while (mRunning)
    {

        // Timer code here

        if (!mMinimized)
        {
            // ImGui code here
        }

        mWindow->OnUpdate();
    }
}
} // namespace Core
