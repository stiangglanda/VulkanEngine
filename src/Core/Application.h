#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include <string>

namespace Core
{

class Application
{
  public:
    Application();

    ~Application();

    void init();
    void run();
    void draw();
    void update();
    void cleanUp();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    constexpr std::string getWindowTitle()
    {
        return mWindowTitle;
    };

  protected:
    uint32_t mWidth = 600;
    uint32_t mHeight = 800;

  private:
    void initWindow();

    GLFWwindow *mWindowPtr;
    bool mResizing = false;
    std::string mWindowTitle = "VulkanEngine";
};
} // namespace Core
