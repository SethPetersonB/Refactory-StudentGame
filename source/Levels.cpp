// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Levels.h"
#include "../include/Logger.h"
#include <fstream>
#include "../include/Stage.h"
#include "../include/StageInit.h"

using namespace Logger;

namespace Engine
{
  using namespace StageInit;

  std::map<std::string, std::unique_ptr<Level> > Level::Levels;

  // array of function pointers to logic of each level
  static void(*Init_functions[])(Stage*) = { LoseInit, WinInit, QuitInit, PauseInit,
                                              HelpInit, SplashInit, GameStageInit, OptionsInit,
                                              CreditsInit, MenuInit, MenuConfirmationInit, RestartInit};
  const Json::Value Level::defValue;

  Level::Level(const std::string& name, const Json::Value& object) : name_(name)
  {
    Json::Value property = object.get("objects", defValue);
    Json::Value index = object.get("index", defValue);
    Log<Info>("%s", name.c_str());

    // Object's component list is not properly deffined
    if (property == defValue || !property.isArray() || index == defValue || !index.isArray())
    {
      Log<Error>("Object of type '%s' is malformed", name.c_str());
    }

    for (unsigned i = 0; i < property.size(); i++)
    {
      // Get component initial values
      std::string propName = property[i].asCString();
      objects.push_back(propName);
    }

    Stage& stage = Stage::New(name);
    index_ = atoi(index[0].asCString());
    //loadInstances(stage);
    loadInstances(stage);

    auto resFunc = 
      [this](Stage* stg) 
    {
      loadInstances(*stg);
      Init_functions[index_](stg);
    };

    Stage::SetResetFunc(stage, std::function<void(Stage*)>(resFunc), false);
  }

  int GenerateLevels(const std::string& levelFile)
  {
    int count = 0;
    std::ifstream inputFile(levelFile.c_str());
    Json::Reader reader;
    Json::Value root;
    const Json::Value & defValue = Level::defValue;

    if (inputFile.is_open())
    {
      reader.parse(inputFile, root);

      std::vector<std::string> objectNames = root.getMemberNames();

      // loop through once to create stages
      for (unsigned i = 0; i < objectNames.size(); i++)
      {
        Json::Value object = root.get(objectNames[i], defValue);

        if (object != defValue)
        {
          Level::Levels[objectNames[i]] = std::unique_ptr<Level>(new Level(objectNames[i], object));
          ++count;
        }
        else
          Log<Error>("Read error while parsing levels!");
      }

      // loop through again to initialize stages
      for (unsigned i = 0; i < objectNames.size(); ++i)
      {
        int j = Level::Levels[objectNames[i]]->index_;
        std::string s = Level::Levels[objectNames[i]]->name_;
        Init_functions[j](&Stage::GetStage(s));
      }
    }
    else
    {
      Log<Error>("Failed to open the level file '%s'", levelFile.c_str());
    }

    return count;
  }
  void Level::loadInstances(Stage & stage) const
  {
    for (auto & instance : objects)
    {
      stage.addGameInstance(instance);
    }
  }
}
