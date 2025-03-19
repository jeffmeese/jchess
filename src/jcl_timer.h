#ifndef JCL_TIMER_H
#define JCL_TIMER_H

#include <chrono>

namespace jcl
{
  
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

//#include "plf_nanotimer.h"

//class TimerHolder
//{
//public:
//  TimerHolder(double & time)
//    : mTime(time)
//  {
//    mTimer.start();
//  }
//
//  ~TimerHolder()
//  {
//    mTime += mTimer.get_elapsed_ms();
//  }

//private:
//  double & mTime;
//  plf::nanotimer mTimer;
//};

}

#endif // TIMER_H