// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <chrono>
#include <iostream>
class Timer
{
public:
  Timer();

  void Reset();
  void Pause();
  void UnPause();

  bool IsPaused() const { return isPaused_; }
  double ElapsedTime() const;

private:
  bool isPaused_;
  std::chrono::milliseconds pauseTime_;
  std::chrono::milliseconds initTime_;
};