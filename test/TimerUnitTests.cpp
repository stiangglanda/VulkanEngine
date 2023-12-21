#include <Timer.h>

#include <gtest/gtest.h>

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
