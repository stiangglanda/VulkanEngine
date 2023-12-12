#include "Platform/Windows/WindowsWindow.h"
#include <Application.h>
#include <Core.h>
#include <GLFW/glfw3.h>
#include <Timer.h>
#include <Window.h>

#include <gtest/gtest.h>

void DisableOnGithubActions()
{
    const char *githubActions = std::getenv("GITHUB_ACTIONS");

    if (githubActions && std::string(githubActions) == "true")
    {
        GTEST_SKIP();
    }
}

// Demonstrate some basic assertions.
TEST(CoreTest, HelloWorldExpectTrue)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(Core::PrintHelloWorld(), true);
}

TEST(CoreTest, Timer)
{
    Core::Timer timer;

    timer.Tick();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.Tick();

    // Expect equality.
    EXPECT_EQ(timer.DeltaTime() > 1 && timer.DeltaTime() < 1.1, true);
}

TEST(CoreTest, TimerStop)
{
    Core::Timer timer;
    std::this_thread::sleep_for(std::chrono::nanoseconds(5));

    timer.Tick();

    timer.Stop();

    timer.Tick();

    // Expect equality.
    EXPECT_EQ(timer.DeltaTime(), 0);
}

TEST(CoreTest, TimerStart)
{
    Core::Timer timer;

    timer.Tick();

    timer.Stop();

    timer.Tick();

    timer.Start();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    timer.Tick();

    // Expect equality.
    EXPECT_EQ(timer.DeltaTime() > 1 && timer.DeltaTime() < 1.1, true);
}

TEST(CoreTest, CreateApplicationHeight)
{
    DisableOnGithubActions();
    // Expect equality.
    EXPECT_EQ(Core::Window::Create()->GetHeight() == 600, true);
}

TEST(CoreTest, CreateApplicationWidth)
{
    DisableOnGithubActions();
    // Expect equality.
    EXPECT_EQ(Core::Window::Create()->GetWidth() == 800, true);
}

TEST(CoreTest, CreateApplicationIsVSync)
{
    DisableOnGithubActions();
    // Expect equality.
    EXPECT_EQ(Core::Window::Create()->IsVSync(), true);
}

TEST(CoreTest, WindowsWindowHeight)
{
    DisableOnGithubActions();
    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

    // Expect equality.
    EXPECT_EQ(window->GetHeight() == 600, true);
}

TEST(CoreTest, WindowsWindowWidth)
{
    DisableOnGithubActions();
    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

    // Expect equality.
    EXPECT_EQ(window->GetWidth() == 800, true);
}

TEST(CoreTest, WindowsWindowIsVSync)
{
    DisableOnGithubActions();
    std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

    // Expect equality.
    EXPECT_EQ(window->IsVSync(), true);
}

TEST(CoreTest, WindowsWindowNativeWindow)
{

    const char *githubActions = std::getenv("GITHUB_ACTIONS");

    if (githubActions && std::string(githubActions) == "true")
    {
        GTEST_SKIP();
    }
    else
    {
        std::unique_ptr<Core::WindowsWindow> window = std::make_unique<Core::WindowsWindow>(Core::WindowProps());

        // Expect equality.
        EXPECT_EQ(static_cast<GLFWwindow *>(window->GetNativeWindow()) != nullptr, true);
    }
}
