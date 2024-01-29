// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Animator.h"
#include <luabind/luabind.hpp>
#include "../include/Stage.h"

namespace Engine
{
  // Event functions

  // Event for when the animation frame is set
  static void OnAnimationFrameSet(Animator * sub, const Packet & payload)
  {
    sub->SetFrame(payload.getData<unsigned>());
  }

  // Event for when the start of the animation is set
  static void OnAnimationStartSet(Animator * sub, const Packet & payload)
  {
    sub->SetStart(payload.getData<unsigned>());
  }

  // Event for when the number of frames is set in an animation
  static void OnAnimationNumFramesSet(Animator * sub, const Packet & payload)
  {
    sub->SetNumFrames(payload.getData<unsigned>());
  }

  // Event for when the hold time for an animation is set
  static void OnAnimationHoldTimeSet(Animator * sub, const Packet & payload)
  {
    sub->SetFrameHold(payload.getData<double>());
  }

  // Event for when an animation is advanced to the next frame
  static void OnAnimationAdvanced(Animator * sub, const Packet & payload)
  {
    sub->Advance();
  }
   
  // Event for when the animation is reset
  static void OnAnimationReset(Animator * sub, const Packet & payload)
  {
    sub->ResetLoop();
  }

  // Event for when the animation is paused
  static void OnAnimationPaused(Animator * sub, const Packet & payload)
  {
    sub->SetPaused(payload.getData<bool>());
  }


  // Requests 

  // Request for animation frame
  static void GetAnimationFrame(const Animator * sub, Packet & data)
  {
    data.setData<size_t>(sub->getCurrentFrame());
  }

  // Requestfor the starting frame of an animation
  static void GetStartFrame(const Animator * sub, Packet & data)
  {
    data.setData<size_t>(sub->getStartFrame());
  }

  // Request for number of frames in an animation
  static void GetNumFrames(const Animator * sub, Packet & data)
  {
    data.setData<size_t>(sub->getNumFrames());
  }

  // Request for hod time for an animation
  static void GetFrameHold(const Animator * sub, Packet & data)
  {
    data.setData<double>(sub->getFrameHold());
  }

  // Request for if an animation is paused
  static void GetFramePaused(const Animator * sub, Packet & data)
  {
    data.setData<bool>(sub->IsPaused());
  }

  /****************************************************************************/
  /*!
    \brief
      Basic constructor for animator component

    \param owner
      Owner of the component
  */
  /****************************************************************************/
  Animator::Animator(GameInstance * owner) :
    Component(owner, "Animator"),
    startFrame_(0), currFrame_(0), 
    numFrames_(0), 
    frameHoldTime_(0),
    paused_(false)
  {
    UpdateFramePos();
  }


  /****************************************************************************/
  /*!
    \brief
      Archetype initialization construtfor for animation component

    \param owner
      Owner of the component
  */
  /****************************************************************************/
  Animator::Animator(GameInstance * owner, const ParsedObject & obj) :
    Component(owner, "Animator"),
    currFrame_(0)
  {
    startFrame_ = obj.getComponentProperty<unsigned>("Animator", "startFrame");
    numFrames_ = obj.getComponentProperty<unsigned>("Animator", "numFrames");
    frameHoldTime_ = obj.getComponentProperty<double>("Animator", "holdTime");
    paused_ = obj.getComponentProperty<bool>("Animator", "startPaused");

    UpdateFramePos();
  }


  /****************************************************************************/
  /*!
    \brief
      Updates the current frame of an objects texture to the current 
      animation fraom
  */
  /****************************************************************************/
  void Animator::UpdateFramePos()
  {
    getParent().getMessenger().Post("TextureFrameSet", startFrame_ + currFrame_);
  }


  /****************************************************************************/
  /*!
    \brief
      Sets the current frame of the animation to the given frame

    \param frame
      Frame to set the animation to
  */
  /****************************************************************************/
  void Animator::SetFrame(size_t frame)
  {
    // Check to make sure we don't divide by 0, and loop animation
    currFrame_ = (numFrames_ > 0) ? frame % numFrames_ : 0;

    UpdateFramePos();
  }


  /****************************************************************************/
  /*!
    \brief
      Sets the starting frame of the animation to the given frame

    \param start
      Starting frame for the animation
  */
  /****************************************************************************/
  void Animator::SetStart(size_t start)
  {
    startFrame_ = start;

    UpdateFramePos();
  }


  /****************************************************************************/
  /*!
    \brief
      Sets the number of frames after the starting point the animation has

    \param num
      Number of frames in the animation
  */
  /****************************************************************************/
  void Animator::SetNumFrames(size_t num)
  {
    numFrames_ = num;

    // If new number is smaller than old number, current frame needs to be updated
    SetFrame(currFrame_);
  }


  /****************************************************************************/
  /*!
    \brief
      Sets the amount of time to hold a frame for before switching to the next

    \param time
      Frame hold time
  */
  /****************************************************************************/
  void Animator::SetFrameHold(double time)
  {
    frameHoldTime_ = time;
  }


  /****************************************************************************/
  /*!
    \brief
      Sets the paused state of the animation

    \param paused
      New paused status
  */
  /****************************************************************************/
  void Animator::SetPaused(bool paused)
  {
    paused_ = paused;
  }

  //****************************************************************************/
  /*!
    \brief
      Advances to the next frame in the animation
  */
  /****************************************************************************/
  void Animator::Advance()
  {
    SetFrame(currFrame_ + 1);
  }


  /****************************************************************************/
  /*!
    \brief
      Resets the animation back to its starting frame
  */
  /****************************************************************************/
  void Animator::ResetLoop()
  {
    SetFrame(0);
  }


  /****************************************************************************/
  /*!
    \brief
      Resets the frame timer
  */
  /****************************************************************************/
  void Animator::ResetFrameTime()
  {
    frameTimer_.Reset();
  }

  // AnimatorHandler

  /****************************************************************************/
  /*!
    \brief
      Constructor for AnimationHandlers

    \param owner
      Stage that owns the handler
  */
  /****************************************************************************/
  AnimatorHandler::AnimatorHandler(Stage * owner) :
    ComponentHandler(owner, "Animator")
  {}


  /****************************************************************************/
  /*!
    \brief
      Update function for animations. Checks current frame timers and advances
      animations if necessary
  */
  /****************************************************************************/
  void AnimatorHandler::update()
  {
    for (auto & component : componentList_)
    {
      Animator * animator = static_cast<Animator *>(component);

      double elapsed = animator->getCurrFrameTime();
      double holdTime = animator->getFrameHold();

      // Advance to the next frame if the current frame is over
      if (!animator->IsPaused() && elapsed > holdTime)
      {
        animator->Advance();
        animator->ResetFrameTime();
      }
    }
  }


  /****************************************************************************/
  /*!
    \brief
      Event connector for animators

    \param base_sub 
      Base component pointer to the animation to connect

  */
  /****************************************************************************/
  void AnimatorHandler::ConnectEvents(Component * base_sub)
  {
    Messenger & objMessenger = base_sub->getParent().getMessenger();
    Animator * sub = static_cast<Animator *>(base_sub);

    REQUEST_ACTION frameReq = std::bind(GetAnimationFrame, sub, std::placeholders::_1);
    REQUEST_ACTION startReq = std::bind(GetStartFrame, sub, std::placeholders::_1);
    REQUEST_ACTION numReq = std::bind(GetNumFrames, sub, std::placeholders::_1);
    REQUEST_ACTION holdReq = std::bind(GetFrameHold, sub, std::placeholders::_1);
    REQUEST_ACTION pauseReq = std::bind(GetFramePaused, sub, std::placeholders::_1);

    objMessenger.SetupRequest("AnimationFrame", frameReq);
    objMessenger.SetupRequest("AnimationStartFrame", startReq);
    objMessenger.SetupRequest("AnimationFrameCount", numReq);
    objMessenger.SetupRequest("AnimationHoldTime", holdReq);
    objMessenger.SetupRequest("AnimationPaused", pauseReq);

    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<size_t>(objMessenger, "SetAnimationFrame"));
    events.push_back(router.newEvent<size_t>(objMessenger, "SetAnimationStart"));
    events.push_back(router.newEvent<size_t>(objMessenger, "SetAnimationFrameCount"));

    events.push_back(router.newEvent<double>(objMessenger, "SetAnimationHoldTime"));

    events.push_back(router.newEvent<bool>(objMessenger, "SetAnimationPaused"));

    // Don't take arguments, but void is invalid as a template type
    events.push_back(router.newEvent<bool>(objMessenger, "AdvanceAnimation"));
    events.push_back(router.newEvent<bool>(objMessenger, "ResetAnimation"));

    sub->getParent().registerScriptEvent(events);
  }

  void AnimatorHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<Animator, Component>("Animator")
      .property("frame", &Animator::getCurrentFrame, &Animator::SetFrame)
      .property("startFrame", &Animator::getStartFrame, &Animator::SetStart)
      .property("frameCount", &Animator::getNumFrames, &Animator::SetNumFrames)
      .property("delay", &Animator::getFrameHold, &Animator::SetFrameHold)
      .property("paused", &Animator::IsPaused, &Animator::SetPaused)

      .def("Reset", &Animator::ResetLoop)
      .def("ResetHold", &Animator::ResetFrameTime)
      .def("Advance", &Animator::Advance)
    );
  }
}