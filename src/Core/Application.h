#pragma once
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include "Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Renderer/RenderAPI.h"
#include "Timer.h"
#include "Window.h"
#include "vepch.h"
#include <GLFW/glfw3.h>
#include "SceneGraph/SceneGraph.h"

int main(int argc, char **argv);

namespace Core
{

struct ApplicationCommandLineArgs
{
    int Count = 0;
    char **Args = nullptr;

    const char *operator[](int index) const
    {
        return Args[index];
    }
};

struct ApplicationSpecification
{
    std::string Name = "Application";
    std::string WorkingDirectory;
    ApplicationCommandLineArgs CommandLineArgs;
};

class Application
{
  public:
    Application(const ApplicationSpecification &specification);
    virtual ~Application();

    void OnEvent(Event &e);

    void Shutdown();

    Window &GetWindow()
    {
        return *mWindow;
    }

    static Application &Get()
    {
        return *s_Instance;
    }
    
    SceneGraph& GetSceneGraph()
    {
        return *mSceneGraph;
    }

    const SceneGraph& GetSceneGraph() const
    {
        return *mSceneGraph;
    }

  private:
    void Run();
    bool OnWindowClose(WindowCloseEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);

    ApplicationSpecification m_Specification;
    static Application *s_Instance;

    std::unique_ptr<Window> mWindow;
    std::unique_ptr<Timer> mTimer;
    std::unique_ptr<SceneGraph> mSceneGraph;
    
    bool mRunning = true;
    bool mMinimized = false;
    friend int ::main(int argc, char **argv);
};
// To be defined in CLIENT
Application *CreateApplication(ApplicationCommandLineArgs args);
} // namespace Core
