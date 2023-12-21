
#include <Window.h>
#include <GLFW/glfw3.h>
#include <Platform/Windows/WindowsWindow.h>

#include <gtest/gtest.h>

#define DisableOnGithubActions                                                                                         \
    {                                                                                                                  \
        const char *githubActions = std::getenv("GITHUB_ACTIONS");                                                     \
                                                                                                                       \
        if (githubActions && std::string(githubActions) == "true")                                                     \
        {                                                                                                              \
            GTEST_SKIP();                                                                                              \
        }                                                                                                              \
    }

TEST(CoreTest, CreateApplicationHeight)
{
    DisableOnGithubActions;
    // Expect equality.
    EXPECT_EQ(Core::Window::Create()->GetHeight() == 600, true);
}

TEST(CoreTest, CreateApplicationWidth)
{
    DisableOnGithubActions;
    // Expect equality.
    EXPECT_EQ(Core::Window::Create()->GetWidth() == 800, true);
}

TEST(CoreTest, CreateApplicationIsVSync)
{
    DisableOnGithubActions;
    // Expect equality.
    EXPECT_EQ(Core::Window::Create()->IsVSync(), true);
}

TEST(CoreTest, WindowsWindowHeight)
{
    DisableOnGithubActions;
    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

    // Expect equality.
    EXPECT_EQ(window->GetHeight() == 600, true);
}

TEST(CoreTest, WindowsWindowWidth)
{
    DisableOnGithubActions;
    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

    // Expect equality.
    EXPECT_EQ(window->GetWidth() == 800, true);
}

TEST(CoreTest, WindowsWindowIsVSync)
{
    DisableOnGithubActions;
    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

    // Expect equality.
    EXPECT_EQ(window->IsVSync(), true);
}

TEST(CoreTest, WindowsWindowNativeWindow)
{
    DisableOnGithubActions;

    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());
    // Expect equality.
    EXPECT_EQ(static_cast<GLFWwindow *>(window->GetNativeWindow()) != nullptr, true);
}