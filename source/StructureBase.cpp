// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include<luabind/luabind.hpp>
#include<luabind/iterator_policy.hpp>

#include "Messages.h"
#include "StructureBase.h"
#include "Stage.h"
#include "sprite.h"

namespace Engine
{
  StructureBase::StructureBase(GameInstance * owner) :
    Component{ owner, "StructureBase" }, typeLoaded_{ false }
  {}

  StructureBase::~StructureBase()
  {
    if (slowed_) ResetColor();
    unloadStruct();
  }

  std::string StructureBase::getStructureType() const
  {
    return typeLoaded_ ? structType_ : "";
  }

  const std::unordered_set<unsigned long>& StructureBase::getInstanceList() const  // Might need to change to pointers
  {
    return instanceIds_;
  }

  void StructureBase::addInstance(unsigned long id)
  {
    instanceIds_.insert(id);
  }

  void StructureBase::removeInstance(unsigned long id)
  {
    instanceIds_.erase(id);
  }

  void StructureBase::loadStructType(const std::string & type)
  {
    unloadStruct();

    typeLoaded_ = true;
    structType_ = type;

    getParent().PostMessage("StructureLoaded", type);
  }

  void StructureBase::unloadStruct()
  {
    if (typeLoaded_)
    {
      typeLoaded_ = false;

      getParent().PostMessage("StructureUnloaded", structType_);
    }
  }

  void StructureBase::SetColor(glm::vec2 pos1, glm::vec2 pos2)
  {
    Grid& grid = getParent().getStage()->GetGrid();
    slowed_ = true;

    for (int j = 0; j < grid.GetGridHeight(); ++j)
    {
      for (int i = 0; i < grid.GetGridWidth(); ++i)
      {
        glm::vec2 tilescale(37.5, 37.5); // 3 days left... I'm hardcoding this
        GameInstance& tile = getParent().getStage()->getInstanceFromID(grid[j][i]);
        glm::vec2 pos = tile.RequestData<glm::vec2>("Position");

        // if tile is in a slow row/ column, change its color
        if ((pos.x >= pos1.x - tilescale.x && pos.x <= pos1.x + tilescale.x) || (pos.y >= pos1.y - tilescale.y && pos.y <= pos1.y + tilescale.y) ||
          (pos.x >= pos2.x - tilescale.x && pos.x <= pos2.x + tilescale.x) || (pos.y >= pos2.y - tilescale.y && pos.y <= pos2.y + tilescale.y))
        {
          glm::vec4 color = tile.RequestData<DrawToken>("Graphic").getShade();
          tile.RequestData<DrawToken>("Graphic").setShade(0.5f * color);
          slowedTiles_.push_back(grid[j][i]);
        }
      }
    }
  }

  void StructureBase::ResetColor()
  {
    for (int i = 0; i < slowedTiles_.size(); ++i)
    {
      GameInstance& tile = getParent().getStage()->getInstanceFromID(slowedTiles_[i]);
      glm::vec4 color = tile.RequestData<DrawToken>("Graphic").getShade();
      tile.RequestData<DrawToken>("Graphic").setShade(2.0 * color);
    }
    slowedTiles_.clear();
  }

  StructureBaseHandler::StructureBaseHandler(Stage * stage) :
    ComponentHandler(stage, "StructureBase")
  {}

  void StructureBaseHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<StructureBase, Component>("StructureBase")
      .def("GetInstanceList", &StructureBase::getInstanceList, return_stl_iterator)
      .def("SetColor", &StructureBase::SetColor)
      .def("ResetColor", &StructureBase::ResetColor)
      .property("type", &StructureBase::getStructureType, &StructureBase::loadStructType)
      .property("isLoaded", &StructureBase::isLoaded)
    );
  }

  void StructureBaseHandler::ConnectEvents(Component * base_sub)
  {
    StructureBase * sub = static_cast<StructureBase *>(base_sub);

    Messenger & objMessenger = sub->getParent().getMessenger();

    // Subscribe CPP events
    REQUEST_ACTION valid = [sub](Packet & data) {data.setData(sub->isLoaded()); };
    REQUEST_ACTION instancesReq = [sub](Packet & data) {data.setData(sub->getInstanceList()); };
    REQUEST_ACTION typeReq = [sub](Packet & data) {data.setData(sub->getStructureType()); };

    SUBSCRIBER_ACTION addInst = [sub](const Packet & data) {sub->addInstance(data.getData<unsigned long>()); };
    SUBSCRIBER_ACTION remInst = [sub](const Packet & data) {sub->removeInstance(data.getData<unsigned long>()); };
    SUBSCRIBER_ACTION unload = [sub](const Packet &) {sub->unloadStruct(); };
    SUBSCRIBER_ACTION load = [sub](const Packet & data) {sub->loadStructType(data.getData<std::string>()); };

    objMessenger.SetupRequest("StructureLoaded", valid);
    objMessenger.SetupRequest("StructureObjects", instancesReq);
    objMessenger.SetupRequest("StructureType", typeReq);

    objMessenger.Subscribe(objMessenger, "LoadStructure", load);
    objMessenger.Subscribe(objMessenger, "UnloadStructure", unload);
    objMessenger.Subscribe(objMessenger, "AddStructureInstance", addInst);
    objMessenger.Subscribe(objMessenger, "RemoveStructureInstance", remInst);
    // Subscribe Lua events
    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<std::string>(objMessenger, "StructureLoaded"));
    events.push_back(router.newEvent<std::string>(objMessenger, "StructureUnloaded"));

    sub->getParent().registerScriptEvent(events);
  }

  void StructureBaseHandler::update()
  {
    for (auto comp : componentList_)
    {
    }
  }

}