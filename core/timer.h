#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
  typedef std::chrono::high_resolution_clock clock;
  typedef std::chrono::high_resolution_clock::time_point time_point;

public:
  Timer(bool startTimer = false);

public:
  double elapsed() const;
  void reset();
  void restart();
  void start();
  void stop();

private:
  time_point mStart;
  double mElapsed;
  bool mStarted;
};

inline double Timer::elapsed() const
{ return mElapsed; }

inline void Timer::reset()
{
  mElapsed = 0.0;
  mStarted = false;
}

inline void Timer::restart()
{
  reset();
  start();
}

inline void Timer::start()
{
  if (mStarted)
    return;

  mStart = clock::now();
  mStarted = true;
}

inline void Timer::stop()
{
  if (!mStarted)
    return;

  time_point current = clock::now();
  mElapsed += std::chrono::duration_cast<std::chrono::microseconds>(current-mStart).count();
  mStarted = false;
}

#include "plf_nanotimer.h"

class TimerHolder
{
public:
  TimerHolder(double & time)
    : mTime(time)
  {
    mTimer.start();
  }

  ~TimerHolder()
  {
    mTime += mTimer.get_elapsed_ms();
  }

private:
  double & mTime;
  plf::nanotimer mTimer;
};

#endif // TIMER_H
