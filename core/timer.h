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

class TimerHolder
{
public:
  TimerHolder(Timer & timer)
    : mTimer(timer)
  {
    mTimer.start();
  }

  ~TimerHolder()
  {
    mTimer.stop();
  }
private:
  Timer & mTimer;
};

#endif // TIMER_H
