#include "jcl_timer.h"

namespace jcl
{
  
Timer::Timer(bool startTimer)
{
  reset();
  if (startTimer)
    start();
}

double Timer::elapsed() const
{ 
  return mElapsed; 
}

void Timer::reset()
{
  mElapsed = 0.0;
  mStarted = false;
}

void Timer::restart()
{
  reset();
  start();
}

void Timer::start()
{
  if (mStarted)
    return;

  mStart = clock::now();
  mStarted = true;
}

void Timer::stop()
{
  if (!mStarted)
    return;

  time_point current = clock::now();
  mElapsed += std::chrono::duration_cast<std::chrono::microseconds>(current-mStart).count();
  mStarted = false;
}

}
