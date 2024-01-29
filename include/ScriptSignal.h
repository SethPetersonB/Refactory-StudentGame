// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <memory>
#include <list>
#include <queue>
#include <string>
#include <functional>

#include "Messages.h"

namespace Engine
{
  class ScriptListener;
  class ScriptEvent;
  class ScriptRouter;

  using SCRIPT_LISTENER_LINK = std::weak_ptr<ScriptListener>;
  using SCRIPT_LISTENER_PTR = std::shared_ptr<ScriptListener>;

  using SCRIPT_EVENT_LINK = std::weak_ptr<ScriptEvent>;
  using SCRIPT_EVENT_PTR = std::shared_ptr<ScriptEvent>;

  class ScriptListener
  {
  public:
    ScriptListener(const ScriptListener &) = delete;
    ScriptListener & operator=(const ScriptListener &) = delete;
    ~ScriptListener();

    void disconnect();

  protected:
    friend class ScriptEvent;
    ScriptListener(ScriptEvent * scriptEvent_, luabind::object _callFunction);
    luabind::object callFunction;
    
  private:
    ScriptEvent * scriptEvent;

  };


  class ScriptEvent
  {
  public:
    ScriptEvent(const ScriptEvent &) = delete;
    ScriptEvent & operator=(const ScriptEvent &) = delete;

    void push();

    template<typename T>
    void postMessage(const Packet & data);

    SCRIPT_LISTENER_PTR connect(luabind::object); /* Might not be correct type. Should be passed a lua function */

    void disconnectListener(ScriptListener * listener);

    std::string eventName() const { return eventName_; };
  protected:
    friend class ScriptRouter;

    template<typename T>
    explicit ScriptEvent(Messenger & sub, lua_State * _L, const std::string & eventName, T**);

  private:
    lua_State * L;
    std::string eventName_;
    luabind::object dataTable;
    unsigned totalObjects_;
    Messenger mess_;

    std::list<SCRIPT_LISTENER_LINK> listeners_;

  };


  class ScriptRouter
  {
  public:
    ScriptRouter(lua_State * _L = nullptr);

    ScriptRouter(const ScriptRouter &) = delete;
    ScriptRouter & operator=(const ScriptRouter &) = delete;

    void update();

    void registerEvent(SCRIPT_EVENT_PTR);

    void reset() { L = nullptr; events_.clear(); }

    template<typename T>
    SCRIPT_EVENT_PTR newEvent(Messenger & sub, const std::string & eventName);

    lua_State * L;

  private:
    std::list<SCRIPT_EVENT_LINK> events_;
  };



  template<typename T>
  ScriptEvent::ScriptEvent(Messenger & sub, lua_State * _L, const std::string & eventName, T **) :
    L(_L), eventName_(eventName), dataTable(luabind::newtable(_L)), totalObjects_(0)
  {
    SUBSCRIBER_ACTION post = std::bind(&ScriptEvent::postMessage<T>, this, std::placeholders::_1);

    mess_.Subscribe(sub, eventName, post);
  }

  template<typename T>
  void ScriptEvent::postMessage(const Packet & data)
  {
    auto iter = listeners_.begin();

    while (iter != listeners_.end())
    {
      if (iter->expired())
        iter = listeners_.erase(iter);
      else
      {
        luabind::call_function<void>(L, "pushEvent", iter->lock()->callFunction, data.getData<T>());
        ++iter;
      }
    }
  }

  // Router
  template<typename T>
  SCRIPT_EVENT_PTR ScriptRouter::newEvent(Messenger & sub, const std::string & eventName)
  {
    // Cannot explictly call templated constructor unless you provide a template parameter of that type
    // Create an uninitialized pointer so that we can provide some type information
    // Actually take a double pointer to prevent uninitialized variable exceptions in debug mode
    T* bogus; 

    SCRIPT_EVENT_PTR returned(new ScriptEvent(sub, L, eventName, &bogus));

    registerEvent(returned);

    return returned;
  }
}


