// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <unordered_set>
#include <vector>

#include "GameInstance.h"
#include "glm/glm/glm.hpp"

namespace Engine
{
  class StructureBase : public Component
  {
  public:

    StructureBase(GameInstance * owner);
    ~StructureBase();

    std::string getStructureType() const;
    const std::unordered_set<unsigned long>& getInstanceList() const;  // Might need to change to pointers
    bool isLoaded() const { return typeLoaded_; }
    void addInstance(unsigned long id);
    void removeInstance(unsigned long id);

    void loadStructType(const std::string & type);
    void unloadStruct();

    void SetColor(glm::vec2 pos1, glm::vec2 pos2);
    void ResetColor();

  private:

    std::unordered_set<unsigned long> instanceIds_;
    std::string structType_;
    bool typeLoaded_;
    std::vector<unsigned long> slowedTiles_;
    bool slowed_ = false;

  };

  class StructureBaseHandler : public ComponentHandler
  {
  public:
    StructureBaseHandler(Stage * stage);

    void update() override;
    void getLuaRegisters() override;

  protected:
    void ConnectEvents(Component * sub) override;

  };
}
