#pragma once
#include <chrono>

namespace Core
{

class Timer
{
  public:
    Timer();

    void Reset();

    float Elapsed();

    float ElapsedMillis()
    {
        return Elapsed() * 1000.0f;
    }

    float DeltaTime() const; // in seconds

    void Start(); // Call when unpaused.
    void Stop();  // Call when paused.
    void Tick();  // Call every frame.

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
    std::chrono::time_point<std::chrono::high_resolution_clock> mStopTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> mPausedTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> mPrevTime;
    float mDeltaTime;

    bool mStopped;
};
} // namespace Core
