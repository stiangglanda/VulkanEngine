#include "Application.h"

namespace Core
{

Application::Application()
{
}

Core::Application::~Application()
{

}

void Application::init()
{
    initWindow();
}

void Application::cleanUp()
{

    glfwDestroyWindow(mWindowPtr);

    glfwTerminate();
}


void Application::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
    app->mResizing = true;
}

void Application::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindowPtr = glfwCreateWindow(mWidth, mHeight, mWindowTitle.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(mWindowPtr, this);
    glfwSetFramebufferSizeCallback(mWindowPtr, framebufferResizeCallback);
}
}