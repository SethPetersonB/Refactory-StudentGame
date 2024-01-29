// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "GSM.h"

namespace Engine
{
  class MenuButtons
  {
  public:
    static void ButtonClickedEvent(GameInstance * button, Stage * previous, Stage * next, const Packet& payload);
    static void InvisibleEvent(GameInstance* g, const Packet& payload);

    static std::map<const std::string, unsigned long> buttons_; // map of buttons' game IDs
  };
}
