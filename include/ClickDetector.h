// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "GameInstance.h"
#include "glm/glm/vec2.hpp"
#include "ParsedObjects.h"
#include <string>

namespace Engine
{
  class Stage;

  class ClickDetector : public Component
  {
  public:
    ClickDetector(GameInstance * owner);
    ClickDetector(GameInstance * owner, const ParsedObject & obj);

    const std::string & getMeshID() const { return meshId; }
  private:
    std::string meshId;
    
  };

  class ClickDetectorHandler : public ComponentHandler
  {
  public:
    ClickDetectorHandler(Stage * stage);

    void update();

    void getLuaRegisters() override {}

  protected:

  protected:
    void ConnectEvents(Component * sub);

  private:
    unsigned long lastMousedOver_;
  };
}