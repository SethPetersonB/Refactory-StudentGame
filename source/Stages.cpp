// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include<luabind/luabind.hpp>
#include<luabind/iterator_policy.hpp>
#include<luabind/operator.hpp>
#include <random>

#include "audio_startup.h"
#include "../include/GSM.h"
#include "../include/Logger.h"
#include "../include/Input.h"
#include "../include/ScriptSignal.h"
#include "temp_utils.hpp"

using namespace Logger;

namespace Engine
{
  std::map<unsigned, std::vector<Stage *> > Stage::StageList;
  Stage* Stage::lastRunning_ = nullptr;

  static void OnStageReset(Stage * stage, const Packet &)
  {
    stage->setStageReset(true);
  }

  /****************************************************************************/
  /*!
  \brief
  Factory for stages. Creates a stage from provided properties

  \param name
  Name of the stage to create

  \param order
  Priority order that the stage initially has. This affects the order
  that it is updated and rendered (0 is first)
  */
  /****************************************************************************/
  Stage & Stage::New(const std::string & name, STAGE_RESET_FUNC reset, unsigned order)
  {
    Stage * newStage = new Stage(name, reset, order);

    return *newStage;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the game grid

    \return 
      Reference to the game grid
  */
  /****************************************************************************/
  //Grid & Stage::GetGrid()
 // {
  //  return GSM::get().getGrid();
 // }


  /****************************************************************************/
  /*!
  \brief
  Gets the first stage from a given name

  \param stageName
  Name of the stage to retrieve

  \return
  Reference to the stage found
  */
  /****************************************************************************/
  Stage & Stage::GetStage(const std::string & stageName)
  {
    for (auto & stage_entry : StageList)
    {
      try
      {
        return GetStage(stageName, stage_entry.first);
      }
      catch (const std::out_of_range&)
      {
      }
    }

    // Only gets here if stage was not found anywhere
    Log<Error>("Stage not with name '%s' found on instance list", stageName.c_str());
    throw std::out_of_range("");
  }

  /****************************************************************************/
  /*!
  \brief
  Gets a stage with the given name on the given priority order

  \param stageName
  Name of the stage to retrieve

  \param order
  Priority order to get the stage from

  \return
  Reference to the stage found
  */
  /****************************************************************************/
  Stage & Stage::GetStage(const std::string & stageName, unsigned order)
  {
    for (auto & stage_ptr : StageList.at(order))
    {
      if (stage_ptr->getStageName() == stageName)
        return *stage_ptr;
    }

    throw std::out_of_range("Stage with the given name was not found on the given order");
  }

  /****************************************************************************/
  /*!
  \brief
  Destroys a stage with the given name and priority order

  \param name
  Name of the stage to destroy

  \param order
  Priority order the stage to destroy exists on
  */
  /****************************************************************************/
  void Stage::DestroyStage(const std::string & name, unsigned order)
  {
    Stage & stag = GetStage(name, order);
    delete &stag;
  }

  /****************************************************************************/
  /*!
  \brief
  Destroys a given stage

  \param stage
  Reference to the stage to destroy
  */
  /****************************************************************************/
  void Stage::DestroyStage(Stage & stage)
  {
    delete &stage;
  }

  /****************************************************************************/
  /*!
    \brief
      Resets a stage by clearing its entities, messenger, and handlers.
      Runs the stage's rest function if it has one.

    \param name
      Name of the stage to reset
     
     \param order
      Order the stage to reset exists on
  */
  /****************************************************************************/
  void Stage::ResetStage(const std::string & name, unsigned order)
  {
    ResetStage(&GetStage(name, order));
  }

  /****************************************************************************/
  /*!
    \brief
      Resets a stage by clearing its entities, messenger, and handlers.
      Runs the stage's rest function if it has one.

    \param stage
      Pointer to the stage to reset
  */
  /****************************************************************************/
  void Stage::ResetStage(Stage * stage)
  {
    stage->stageClear();
    stage->setStageReset(false);
    stage->getMessenger().Clear();

    stage->initLuaSandbox();
    stage->startLuaSandbox();

    if (stage->resFunc_)
      stage->resFunc_(stage);
  }

  /****************************************************************************/
  /*!
    \brief
      Gets a unique ID for stages

    \return
      Unsigned long to use for the ID
  */
  /****************************************************************************/
  unsigned long Stage::AssignID()
  {
    static unsigned long nextId = 0;

    return ++nextId;
  }

  /****************************************************************************/
  /*!
  \brief
  Constructor for stages. Creates a stage from a name and a priority order

  \param name
  Name to give to the stage

  \param order
  Order to create the stage in
  */
  /****************************************************************************/
  Stage::Stage(const std::string & name, STAGE_RESET_FUNC reset, unsigned order) :
    stageName_(name), stageId_(AssignID()), resetting_(false), resFunc_(reset),
    lua_Sandbox_(nullptr)
  {
    stageOrder_ = order;

    try
    {
      GetStage(name, order);

      // Only gets here if another stage with the same name exists on the same
      // order
      Log<Error>("Attempting to duplicate a stage with the name '%s' on order %d",
        name.c_str(), order);
      throw stage_duplication();
    }
    catch (const std::out_of_range &)
    {
      StageList[order].push_back(this);
      isRunning_ = true;
      isRendered_ = true;
      stageEnding_ = false;
      toggleRunning_ = false;
      testVar = 100;
      initHandlers();

      //initLuaSandbox();

      using namespace std::placeholders;

      SUBSCRIBER_ACTION onReset = std::bind(OnStageReset, this, std::placeholders::_1);

      getMessenger().Subscribe(getMessenger(), "STAGE_RESET", onReset);

      initLuaSandbox();
      startLuaSandbox();

      if (resFunc_)
        resFunc_(this);

      SUBSCRIBER_ACTION quitConfirm =
        [this, name] (const Packet& payload) mutable
      {
        if (name == "QuitStage")
          this->setStageRunning(true);

        else
        {
          if (this->isStageRunning())
            Stage::lastRunning_ = this;

          this->setStageRunning(false);
        }
      };

      getMessenger().Subscribe(GSM::get().getMessenger(), "QUIT_CONFIRM", quitConfirm);
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the reset functon of a stage

    \param stage
      Stage to set the reset function for

    \param res
      Function to set 

    \param resetNow
      Whether to set the stage to reset immediatly (next game loop) or not
  */
  /****************************************************************************/
  void Stage::SetResetFunc(Stage & stage, STAGE_RESET_FUNC res, bool resetNow)
  {
    stage.resFunc_ = res;

    // Have to do conditional check. Stage may already be resetting and 
    // you probably don't want to overwrite that
    if (resetNow)
      stage.setStageReset(true);
  }

  /****************************************************************************/
  /*!
  \brief
  Destructor for stages. Frees all entities and handlers from memory
  */
  /****************************************************************************/
  Stage::~Stage()
  {
    stageClear();
  }

  /****************************************************************************/
  /*!
    \brief
      Clears all gameinstances, handlers, and messages on a stage
  */
  /****************************************************************************/
  void Stage::stageClear()
  {
    flushInstanceList();
    flushHandlers();
    getMessenger().Clear();
  }

  /****************************************************************************/
  /*!
  \brief
  Sets a stage to running or not running (determines if it gets updated
  or not each loop)

  \param running
  Boolean. True for running, false for paused
  */
  /****************************************************************************/
  void Stage::setStageRunning(bool running)
  {
    if (running != isRunning_)
    {
      toggleRunning_ = true;

      mess_.Post("STAGE_PAUSED", Message<bool>(running));
    }
  }

  void Stage::ToggleRunning(Stage & stage)
  {
    stage.isRunning_ = !stage.isRunning_;
    stage.toggleRunning_ = false;
  }
  /****************************************************************************/
  /*!
  \brief
  Sets the priority order a stage exists on

  \order
  Priority order to set the stage on
  */
  /****************************************************************************/
  void Stage::setStageOrder(unsigned order)
  {
    // Make sure that the current stage exists on the stage list. If not,
    // that is a fatal error
    try
    {
      Stage& currStage = GetStage(getStageName(), getStageOrder());

      if (&currStage != this)
      {
        Log<Error>("Stage list is malformed!");
        throw malformed_stage_list();
      }
    }
    catch (const std::out_of_range&)
    {
      Log<Error>("Stage list is malformed!");
      throw malformed_stage_list();
    }

    try
    {
      // Only succeeds if a stage with the same name already exists at 
      // the new order
      GetStage(getStageName(), order);

      throw stage_duplication();
    }
    catch (const std::out_of_range&)
    {
      // Vector iterator swap and pop to remove the stage from it's current order
      for (auto stage_order_iter = StageList[getStageOrder()].begin(); stage_order_iter != StageList[getStageOrder()].end(); ++stage_order_iter)
      {
        if (*stage_order_iter == this)
        {
          // Swap the stage to the end of the order
          std::swap(*stage_order_iter, *(StageList[getStageOrder()].end() - 1));

          // pop the order vector
          StageList[getStageOrder()].pop_back();

          break;
        }
      }

      // Put the stage onto the new order
      StageList[order].push_back(this);

      // get rid of the old stage order if there are no longer any stages in it
      if (StageList[getStageOrder()].size() <= 0)
      {
        StageList.erase(getStageOrder());
      }

      // Set the stageOrder_ variable
      stageOrder_ = order;
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Sets a stage to rendered or not rendered (determines if it gets drawn
  or not each loop)

  \param running
  Boolean. True for rendered, false for not rendered
  */
  /****************************************************************************/
  void Stage::setStageRendered(bool rendered)
  {
    isRendered_ = rendered;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets the name of a stage

  \return
  Constant string reference to the stages name
  */
  /****************************************************************************/
  const std::string & Stage::getStageName() const
  {
    return stageName_;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets if the stage is currently running or not

  \return
  True if the stage is running, false if it isn't
  */
  /****************************************************************************/
  bool Stage::isStageRunning() const
  {
    return isRunning_;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets if the stage is currently rendered or not

  \return
  True if the stage is rendered, false if it isn't
  */
  /****************************************************************************/
  bool Stage::isStageRendered() const
  {
    return isRendered_;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets the stage's priority order

  \return
  Order the stage exists on
  */
  /****************************************************************************/
  unsigned Stage::getStageOrder() const
  {
    return stageOrder_;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets if the stage is set up to end at the end of the frame

  \return
  True if the stage is ending, false if it isn't
  */
  /****************************************************************************/
  bool Stage::isStageEnding() const
  {
    return stageEnding_;
  }

  /****************************************************************************/
  /*!
  \brief
  Sets the stage's priority order to 0
  */
  /****************************************************************************/
  void Stage::sendStageToBack()
  {
    setStageOrder(0);
  }

  /****************************************************************************/
  /*!
  \brief
  Sets a stage's oriority order to the last priority order that currently
  exists (updated last, drawn in the front)
  */
  /****************************************************************************/
  int Stage::bringStageToFront()
  {
    setStageOrder((--StageList.end())->first);
    return (--StageList.end())->first;
  }

  /****************************************************************************/
  /*!
    \brief
      Sets a stage's ending flag to signal that the stage should end
  */
  /****************************************************************************/
  void Stage::stageEnd()
  {
    stageEnding_ = true;
  }

  // Thrown when the stage list has become malformed
  const char * Stage::malformed_stage_list::what() const throw()
  {
    return "The stage list has become corrupted!";
  }

  // Thrown when attempting to duplicate a stage on an order
  const char * Stage::stage_duplication::what() const throw()
  {
    return "Attempting to insert or move a stage into an order that already has a stage of the same name!";
  }



  /****************************************************************************/
  /*!
  \brief
  Runs all update logic on a stage
  */
  /****************************************************************************/
  void Stage::update()
  {      
      //event_Router_.update();
//    addGameInstance("Box0");
      updateHandlers();
      burstScripts(GSM::get().getDisplay().GetFrameTime());
  }

  void Stage::addHierarchy(GameInstance * inst)
  {
    using namespace luabind;

    if (lua_Sandbox_)
    {
      std::string type = inst->getObjectType();

      if (!gettable(hierarchy_, type))
        (hierarchy_)[type] = luabind::newtable(lua_Sandbox_->getLuaState());

      (hierarchy_)[type][inst->getId()] = inst->getHierarchy();
      hierarchy_[inst->getId()] = inst->getHierarchy();
    }
  }


  void Stage::removeHierarchy(const std::string & type, unsigned long id)
  {
    using namespace luabind;

    if (lua_Sandbox_)
    {
      if (gettable(hierarchy_, type))
        (hierarchy_)[type][id] = nil;

      if (gettable(hierarchy_, id))
        hierarchy_[id] = nil;


    }
  }

  /****************************************************************************/
  /*!
  \brief
  Adds a game object to a stage with the given type

  \param type
  Archetype to create the object from

  \return
  Reference to the created object
  */
  /****************************************************************************/
  GameInstance & Stage::addGameInstance(const std::string type)
  {
    GameInstance * newInst = GameInstance::New(this, type);

    gameInstanceList_[newInst->getId()] = newInst;

    // Add to lua hierarchy
    addHierarchy(newInst);

    // Initialize lua scripts
    newInst->initScripts();

    return *newInst;
  }

  /****************************************************************************/
  /*!
  \brief
  Adds a basic game object to a stage

  \return
  Reference to the created object
  */
  /****************************************************************************/
  GameInstance & Stage::addGameInstance()
  {
    GameInstance * newInst = GameInstance::New(this);

    gameInstanceList_[newInst->getId()] = newInst;

    // Add to lua hierarchy
    addHierarchy(newInst);

    // Initialize lua scripts
    newInst->initScripts();

    return *newInst;
  }

  /****************************************************************************/
  /*!
  \brief
  removes a GameInstance with the given ID from the stage

  \param id
  ID of the object to remove
  */
  /****************************************************************************/
  void Stage::removeGameInstance(unsigned long id)
  {
    removed_.insert(id);

    // Set hierarchy to nil
    // TODO: determine if this needs to go in CleanStage, or if it's fine here

  }
  
  void Stage::CleanStage(Stage & stage)
  {
    for(auto & id : stage.removed_)
    {
      try
      {
        GameInstance * inst = stage.gameInstanceList_.at(id);
        std::string type = inst->getObjectType();
        unsigned long id = inst->getId();

        delete stage.gameInstanceList_.at(id);
        stage.removeHierarchy(type, id);

      }
      catch (const std::out_of_range &) {}

      stage.gameInstanceList_.erase(id);
    }
    stage.removed_.clear();

  }
  /****************************************************************************/
  /*!
  \brief
  removes a GameInstance wifrom the stage

  \param inst
  Reference to the object to remove
  */
  /****************************************************************************/
  void Stage::removeGameInstance(GameInstance & inst)
  {
    removeGameInstance(inst.getId());
  }

  /****************************************************************************/
  /*!
  \brief
  Removes all GameInstances from the stage and frees their memory
  */
  /****************************************************************************/
  void Stage::flushInstanceList()
  {
    // Delete instances in reveres order
    auto it = gameInstanceList_.rbegin();
    for (; it != gameInstanceList_.rend(); it++ )
    {
      GameInstance * inst = it->second;

      std::string name = inst->getObjectType();
      unsigned long id = inst->getId();

      delete inst;

      removeHierarchy(name, id);
    }

    gameInstanceList_.clear();
  }

  /****************************************************************************/
  /*!
  \brief
  Gets a refference to a GameInstance from the provide ID

  \param id
  ID of the instance to get

  \return
  reference to the gameInstance that was found
  */
  /****************************************************************************/
  GameInstance & Stage::getInstanceFromID(unsigned long id) const
  {
    return *(gameInstanceList_.at(id));
  }

  GameInstance & Stage::getFirstInstanceByName(const std::string & name) const
  {
    for (auto& instance : gameInstanceList_)
    {
      if (instance.second->getObjectType() == name)
        return *instance.second;
    }

    throw std::out_of_range("No instance found with the given name");
  }

  /****************************************************************************/
  /*!
  \brief
  Prints the isntance list of a stage to the provide ostream. Primarily for
  debugging purposes

  \param os
  Stream to print into

  \return
  Reference to the stream that was printed into
  */
  /****************************************************************************/
  std::ostream & Stage::printInstanceList(std::ostream & os) const
  {
    os << "< ";

    for (auto& map_entry : gameInstanceList_)
    {
      os << (map_entry.second)->getObjectType();
    }

    os << " > ";

    return os;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets a handler of a given type on a stage

  \param type
  Type of handler to retrieve

  \return
  Pointer to the handler that was found
  */
  /****************************************************************************/
  ComponentHandler * Stage::getHandler(const std::string & type) const
  {
    for (auto & handler_ptr : handlers_)
    {
      if (handler_ptr->getType() == type)
        return handler_ptr;
    }
    
    // only gets here if it did not find the desired handler
    throw component_handler_not_found();
  }

  /****************************************************************************/
  /*!
  \brief
  Creates a component of the given type and adds it to the given instance

  \param owner
  Pointer to the GameInstance to add the component to

  \param type
  Type of component to create

  \return
  Pointer to the component that was created
  */
  /****************************************************************************/
  Component * Stage::createComponentFromType(GameInstance * owner,
    const std::string & type)
  {
    return fetchHandler(type)->createComponent(type, owner);
  }

  /****************************************************************************/
  /*!
  \brief
  Creates a component of the given type and adds it to the given instance.
  Initializes the component to the parameters provided by an archetype

  \param owner
  Pointer to the GameInstance to add the component to

  \param type
  Type of component to create

  \param obj
  Archetype to initialize a component to

  \return
  Pointer to the component that was created
  */
  /****************************************************************************/
  Component * Stage::createComponentFromType(GameInstance * owner,
    const std::string & type,
    const ParsedObject & obj)
  {
    return  fetchHandler(type)->createComponent(type, owner, obj);
  }

  /****************************************************************************/
  /*!
  \brief
  Flushes all component handlers on a stage from memory
  */
  /****************************************************************************/
  void Stage::flushHandlers()
  {
    for (auto& handler_ptr : handlers_)
    {
      delete handler_ptr;
    }

    handlers_.clear();
  }

  /****************************************************************************/
  /*!
  \brief
  Updates all component handlers on a stage
  */
  /****************************************************************************/
  void Stage::updateHandlers()
  {
    for (unsigned i = 0; i < handlers_.size(); i++)
    {
      if((isRunning_ && !toggleRunning_) || handlers_[i]->isPausable() == false)
        handlers_[i]->updateComponents();
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Initializes all component handlers needed by a stage (TODO, get a
  better system for doing this)
  */
  /****************************************************************************/
  void Stage::initHandlers()
  {
    //fetchHandler("Transform");
    //fetchHandler("Physics");
    //fetchHandler("Collider");
    //fetchHandler("Sprite");
  }

  /****************************************************************************/
  /*!
  \brief
  Adds a component handler to the stage

  \param handler
  Pointer to the handler to add to the stage
  */
  /****************************************************************************/
  void Stage::addHandler(ComponentHandler * handler)
  {
    try
    {
      getHandler(handler->getType());

      // If a handler already exists for this type, throw an exception
      throw component_handler_duplication();
    }
    catch (const std::exception&)
    {
      // Otherwise add it to the handler list
      handlers_.push_back(handler);
     // handler->getLuaRegisters();
    }
  }

  void Stage::FlushStageList()
  {
    for (auto& stage_order_entry : Stage::StageList)
    {
      for (auto & stage_ptr : stage_order_entry.second)
      {
        Stage::DestroyStage(*stage_ptr);
      }

      stage_order_entry.second.clear();
    }

    StageList.clear();
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the component handler of the desired type if one exists. Creates a
      new handler of the desired type if one doesn't yet exist

    \param type
      Type of handler to create or retrieve

    \return 
      Pointer to a handler of the desired type
  */
  /****************************************************************************/
  ComponentHandler * Stage::fetchHandler(const std::string & type)
  {
    try
    {
      return getHandler(type);
    }
    catch (component_handler_not_found&)
    {
      Log<Info>("Adding handler of type '%s'", type.c_str());

      return ComponentHandler::CreateHandler(type, this);
    }
  }

  // Output operator for vec2 for use in lua
  std::ostream & operator<<(std::ostream & os, const glm::vec2 & rhs)
  {
    os << rhs.x << ", " << rhs.y;

    return os;
  }

  static int add_file_and_line(lua_State* L)
  {
    lua_Debug d;
    lua_getstack(L, 1, &d);
    lua_getinfo(L, "Sln", &d);
    std::string err = lua_tostring(L, -1);
    lua_pop(L, 1);
    std::stringstream msg;
    msg << d.short_src << ":" << d.currentline;

    if (d.name != 0)
    {
      msg << "(" << d.namewhat << " " << d.name << ")";
    }
    msg << " " << err;
    lua_pushstring(L, msg.str().c_str());
    return 1;
  }

  static glm::vec2 lua_vec_unit(const glm::vec2 & vec)
  {
    return (1.0f / sqrt((vec.x * vec.x) + (vec.y * vec.y))) * vec;
  }

  static float lua_vec_mag(const glm::vec2 & vec)
  {
    return sqrt((vec.x * vec.x) + (vec.y * vec.y));
  }

  void Stage::initLuaSandbox(const std::string & sandbox)
  {
    using namespace luabind;

    Sandbox * newBox = new Sandbox(sandbox);

    hierarchy_ = newtable(newBox->getLuaState());

    event_Router_.reset();

    // Game state
    lua_Sandbox_.reset(newBox);

    event_Router_.L = lua_Sandbox_->getLuaState();

    (hierarchy_)["name"] = stageName_;

    // Register essential components with the sandbox (GSM, input, etc)
    //registerLuaModule(class_<GSM>("gameState").property("frametime", &GSM::getFrameTime));
    //registerLuaModule(def("fetchGameState", &GSM::get));

    // GameInstance
    registerLuaModule(
      class_<GameInstance>("GameInstance")  // Register GameInstance to Lua
      .def("GetType", &GameInstance::getObjectType)
      .def("GetComponentList", &GameInstance::getComponentList, return_stl_iterator)
      .def("GetEventsList", &GameInstance::getScriptEvents, return_stl_iterator)
      .def("FindComponent", &GameInstance::getComponent)
      .def("GetID", &GameInstance::getId)
      .def("GetHierarchy", &GameInstance::getHierarchy)
    );

    registerLuaModule(
      class_<Stage>("Stage")  // Register Stage with lua
      .def("NewInstance", (GameInstance&(Stage::*)(const std::string))&Stage::addGameInstance)
      .def("RemoveInstance", (void(Stage::*)(GameInstance&))&Stage::removeGameInstance)
      .def("IsRunning", &Stage::isStageRunning)
      .def_readonly("hierarchy", &Stage::hierarchy_)
    );

    //Component
    registerLuaModule(
      class_<Component>("Component")  // Register basic component with Lua
      .property("parent", &Component::getParent)
      .def("GetType", &Component::getComponentType));
      
    // Systems
    registerLuaModule(
      namespace_("systems")[
        InputSystem::GetLuaRegisters(),  // Register input system with Lua
          Audio_Engine::GetLuaRegisters(),
          Camera::GetLuaRegisters(),
          InputSystem::GetLuaRegistersMouse()
      ]);

    //Misc classes
    registerLuaModule(
      class_<glm::vec2>("Vector2")
        .def(constructor<float, float>())
        .def(const_self + glm::vec2())
        .def(const_self - glm::vec2())
        .def(const_self * float())
        .def(float() * const_self)
        .def(-const_self)
        .def(const_self == glm::vec2())
        //.def(tostring(const_self))
        
        .property("magnitude", &lua_vec_mag)
        .property("unit", &lua_vec_unit)
        .def_readonly("x", &glm::vec2::x)
        .def_readonly("y", &glm::vec2::y)
    );

    registerLuaModule(
      class_<glm::vec4>("Vector4")
      .def(constructor<float, float, float, float>())
      .def(const_self + glm::vec4())
      .def(const_self - glm::vec4())
      .def(const_self * float())
      .def(float() * const_self)
      .def(-const_self)
      .def(const_self == glm::vec4())
      //.def(tostring(const_self))

      .def_readonly("r", &glm::vec4::r)
      .def_readonly("g", &glm::vec4::g)
      .def_readonly("b", &glm::vec4::b)
      .def_readonly("a", &glm::vec4::a)
    );

    registerLuaModule(
      class_<ScriptEvent, std::shared_ptr<ScriptEvent>>("ScriptEvent")
      .def("Connect", &ScriptEvent::connect)
      .property("Name", &ScriptEvent::eventName));

    registerLuaModule(
      class_<ScriptListener, std::shared_ptr<ScriptListener>>("ScriptListener")
      .def("Disconnect", &ScriptListener::disconnect));

    registerLuaModule(
      namespace_("math")[
        def("random_normal", &Utils::random_normal<double>),
          def("random_uniform", &Utils::random_uniform<double>)
      ]
    );

    hierarchy_["_RAW"] = this;

  }

  void Stage::unloadHierarchyInstance(GameInstance * obj)
  {
    (hierarchy_)[obj->getObjectType()][obj->getId()] = luabind::nil;
    (hierarchy_)[obj->getId()] = luabind::nil;
  }

  void Stage::startLuaSandbox()
  {
    if (lua_Sandbox_)
    {
      lua_Sandbox_->start();

      // Initialize hierarchy
      luabind::call_function<void>(lua_Sandbox_->getLuaState(), "init", this);
    }
  }
  SCRIPT_PTR Stage::loadScript(const std::string & script)
  {
    if (lua_Sandbox_)
      return lua_Sandbox_->loadScript(script);
    else
      throw std::runtime_error("Atempted to load a script on a stage with no sandbox loaded");
  }

  SCRIPT_PTR Stage::loadScript(const std::string & script, GameInstance * owner)
  {
    if (lua_Sandbox_)
      return lua_Sandbox_->loadScript(script, owner);
    else
      throw std::runtime_error("Atempted to load a script on a stage with no sandbox loaded");
  }


  void Stage::unloadScript(SCRIPT_PTR script)
  {
    if (lua_Sandbox_)
      lua_Sandbox_->unloadScript(script);
  }


  void Stage::burstScripts(double dt)
  {
    if (lua_Sandbox_)
    {
      try {
        lua_Sandbox_->update(GSM::get().getDisplay().GetFrameTime());
      }
      catch (const std::exception & excep){
        Log<Error>(excep.what());
      }
    }
  }

  void Stage::registerLuaModule(luabind::scope & mod)
  {
    using namespace luabind;

    if (lua_Sandbox_)
      module(lua_Sandbox_->getLuaState())[mod];
    else
      Log<Error>("Cannot register lua module; no sandbox loaded!");
  }

  void Stage::registerLuaModule(std::vector<luabind::scope> mods)
  {
    for (auto & mod : mods)
      registerLuaModule(mod);
  }
}

