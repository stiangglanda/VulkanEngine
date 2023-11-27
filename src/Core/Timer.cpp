#include "Timer.h"

namespace Core
{

Timer::Timer()
{
    Reset();
}

void Timer::Reset()
{
    mStart = std::chrono::high_resolution_clock::now();
    mPrevTime = mStart;
    mStopped = false;
}

float Timer::Elapsed()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart)
               .count() *
           0.001f * 0.001f * 0.001f;
}

float Timer::DeltaTime() const
{
    return mDeltaTime;
}

void Timer::Stop()
{
    if (!mStopped)
    {
        mStopTime = std::chrono::high_resolution_clock::now();
        mStopped = true;
    }
}

void Timer::Start()
{
    if (mStopped)
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> currTime =
            std::chrono::high_resolution_clock::now();
        mPausedTime += (currTime - mStopTime);

        mPrevTime = currTime;
        mStopped = false;
    }
}

void Timer::Tick()
{
    if (mStopped)
    {
        mDeltaTime = 0;
        return;
    }

    // Time difference between this frame and the previous.
    std::chrono::time_point<std::chrono::high_resolution_clock> currTime = std::chrono::high_resolution_clock::now();
    auto tDiff = std::chrono::duration<double, std::milli>(currTime - mPrevTime).count();
    mDeltaTime = (float)tDiff / 1000.0f;

    // Prepare for next frame.
    mPrevTime = currTime;

    // Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the
    // processor goes into a power save mode or we get shuffled to another
    // processor, then mDeltaTime can be negative.
    if (mDeltaTime < 0.0)
    {
        mDeltaTime = 0.0;
    }
}

} // namespace Core
