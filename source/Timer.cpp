// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Timer.h"

using namespace std::chrono;

/****************************************************************************/
/*!
  \brief
    Constructor for the Timer class
*/
/****************************************************************************/
Timer::Timer()
{
  Reset();
}

/****************************************************************************/
/*!
  \brief
    Resets the timer to 0.0 seconds
*/
/****************************************************************************/
void Timer::Reset()
{
  isPaused_ = false;
  initTime_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

/****************************************************************************/
/*!
  \brief
    Toggles the pause on the timer

  \return
    New paused status of the timer
*/
/****************************************************************************/
void Timer::Pause()
{
  
  // Set paused time to the current elapsed time since initialization
  if (!isPaused_)
    pauseTime_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - initTime_;

  isPaused_ = true;

}

void Timer::UnPause()
{
  if(isPaused_)
    initTime_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - pauseTime_;

  isPaused_ = false;
}

/****************************************************************************/
/*!
  \brief
    Gets the time in seconds that have elapsed since the last reset of the
    timer

  \return
    Double representing the numbe of secodns since last reset
*/
/****************************************************************************/
double Timer::ElapsedTime() const
{
  if(isPaused_)
    return (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - pauseTime_).count() / 1000.0;
  else
    return (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - initTime_).count() / 1000.0;
}
