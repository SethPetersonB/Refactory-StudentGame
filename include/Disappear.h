// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "Input.h"

namespace Engine
{
  class Disappear : public Component
  {
  public:
    Disappear(GameInstance* owner);
    Disappear(GameInstance* owner, const ParsedObject& obj);
    virtual ~Disappear() {};

    bool IsVisible() const;
    SDL_Scancode Key() const;
    bool TwoWay() const;
    bool PlacementTrigger() const;

    void SetVisible(bool visible);

  private:
    friend class DisappearHandler;
    SDL_Scancode key_; // when this is pressed, object turns invisible
    bool visible_ = true; // if object is currently visible
    bool two_way_ = false; // if object should disappear and reappear when key is pressed
    bool placement_trigger_ = false; // if object should disappear when tower is placed
  };

  class DisappearHandler : public ComponentHandler
  {
  public:
    DisappearHandler(Stage* stage);
    virtual ~DisappearHandler() {};
    void update();
    void getLuaRegisters() override;

  protected:
    void ConnectEvents(Component * base_sub);
  };

}
