#include "Application.h"

namespace Core
{

Application *Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification &specification) : m_Specification(specification)
{
    VE_CORE_INFO("Application Constructor start");
    s_Instance = this;
    // if (!m_Specification.WorkingDirectory.empty())
    //     std::filesystem::current_path(m_Specification.WorkingDirectory);

    mWindow = Window::Create(WindowProps(m_Specification.Name));
    m_RenderAPI = RenderAPI::Create(mWindow->GetNativeWindow());
    m_RenderAPI->Init();
    mWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    mTimer = std::make_unique<Timer>();
    mTimer->Start();
    VE_CORE_INFO("Application Constructor end");
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
    VE_CORE_INFO("OnWindowClose");
    mRunning = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent &e)
{
    VE_CORE_INFO("OnWindowResize");
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
    VE_CORE_INFO("Shutdown");
    m_RenderAPI->Shutdown();
}

void Application::Run()
{
    VE_CORE_INFO("Application Run Start");
    while (mRunning)
    {

        // Timer code here

        if (!mMinimized)
        {
            // ImGui code here
        }

        mWindow->OnUpdate();

        m_RenderAPI->Update(mTimer->DeltaTime());
        m_RenderAPI->Draw();
        mTimer->Tick();
    }
    VE_CORE_INFO("Application Run End");
}
} // namespace Core
