// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "Timer.h"

namespace Engine
{
  class Animator : public Component
  {
  public:
    Animator(GameInstance * owner);
    Animator(GameInstance * owner, const ParsedObject & obj);

    size_t getCurrentFrame() const { return currFrame_; }
    size_t getStartFrame() const { return startFrame_; }
    size_t getNumFrames() const { return numFrames_; }
    double getFrameHold() const { return frameHoldTime_; }
    double getCurrFrameTime() const { return frameTimer_.ElapsedTime(); }
    bool IsPaused() const { return paused_; }

    void SetFrame(size_t frame);
    void SetStart(size_t start);
    void SetNumFrames(size_t num);
    void SetFrameHold(double time);
    void SetPaused(bool paused);
    
    void Advance();
    void ResetLoop();
    void ResetFrameTime();
  

  protected:
    void UpdateFramePos();

  private:
    size_t startFrame_;
    size_t currFrame_;
    size_t numFrames_;
    double frameHoldTime_;
    Timer frameTimer_;
    bool paused_;
  };

  class AnimatorHandler : public ComponentHandler
  {
  public:
    AnimatorHandler(Stage * owner);

    void update();

    void getLuaRegisters() override;

    void ConnectEvents(Component * base_sub);
  };
}
