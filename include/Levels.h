// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <string>
#include "json/json.h"
#include <utility>
#include <memory>
#include <vector>
#include "Stage.h"

namespace Engine
{
  int GenerateLevels(const std::string& levelFile);

  struct Level
  {
    Level(const std::string& name, const Json::Value& object);
    static std::map<std::string, std::unique_ptr<Level> > Levels;

    void loadInstances( Stage & stage) const;
    const std::string name_;
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
    std::vector<std::string> objects;
    static const Json::Value defValue;
    int index_;
  };
}
