#include "Application.h"

#include <iostream>

namespace Core
{

Application *Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification &specification) : m_Specification(specification)
{
    s_Instance = this;
    // if (!m_Specification.WorkingDirectory.empty())
    //     std::filesystem::current_path(m_Specification.WorkingDirectory);

    mWindow = Window::Create(WindowProps(m_Specification.Name));
}

Core::Application::~Application()
{
    Shutdown();
}

void Application::Shutdown()
{

    // glfwDestroyWindow(mWindow);

    // glfwTerminate();
}


void Application::Run()
{
    std::string input;//TODO delete
    std::cin >> input;
} // glfwInit();

// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

// mWindowPtr = glfwCreateWindow(mWidth, mHeight, mWindowTitle.c_str(), nullptr, nullptr);
// glfwSetWindowUserPointer(mWindowPtr, this);
// glfwSetFramebufferSizeCallback(mWindowPtr, framebufferResizeCallback);

} // namespace Core
