/*!
 * \file jcl_timer.h
 *
 * This file contains the interface for the Timer object
 */

#ifndef JCL_TIMER_H
#define JCL_TIMER_H

#include <chrono>

namespace jcl
{

/*!
 * \brief Defines a high resolution timer
 *
 * The Timer object defines a high resolution timer
 * that can be used to measure performance of particular
 * algorithms or routines.
 */
class Timer
{
public:

  /*!
   * \brief Constuctor
   *
   * This function constructs a timer object. The
   * startTimer parameter detemines whether the
   * timer will start immediately after construction.
   * If this parameter is false, the default, the timer
   * can be started with the start function.
   *
   * \param startTimer true to start the timer immediately
   */
  Timer(bool startTimer = false);

  /*!
   * \brief Returns the elapsed time
   *
   * This function returns the amount of time that has elasped
   * since the timer was started.
   *
   * \return The elapsed time
   */
  double elapsed() const;

  /*!
   * \brief Resets the timer
   *
   * This function resets the timer to the initial state without
   * restarting the timer.
   *
   */
  void reset();

  /*!
   * \brief Restarts the timer
   *
   * This function resets the timer to the initial state and
   * then starts the timer again.
   *
   */
  void restart();

  /*!
   * \brief Starts the timer
   *
   * This function starts the timer running
   *
   */
  void start();

  /*!
   * \brief Stops the timer
   *
   * This function stops the timer.
   *
   */
  void stop();

private:

  using clock = std::chrono::high_resolution_clock;
  using time_point = std::chrono::high_resolution_clock::time_point;

  time_point mStart;
  double mElapsed;
  bool mStarted;
};

class AutoTimer
{
public:
  AutoTimer(Timer & timer)
      : mTimer(timer)
  {
    mElapsed = 0.0;
    mTimer.start();
  }

  ~AutoTimer()
  {
    mElapsed += mTimer.elapsed();
  }

private:
  Timer & mTimer;
  double mElapsed;
};
}

#endif // TIMER_H
