// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/ScriptSignal.h"

namespace Engine
{
  // Listeners
  ScriptListener::ScriptListener(ScriptEvent * scriptEvent_, luabind::object _callFunction) :
    callFunction(_callFunction), scriptEvent(scriptEvent_)
  {}

  ScriptListener::~ScriptListener()
  {
    disconnect();
  }

  void ScriptListener::disconnect()
  {
    if (scriptEvent != nullptr)
    {
      scriptEvent->disconnectListener(this);
      scriptEvent = nullptr;
    }
  }

  void ScriptEvent::push()
  {

    auto iter = listeners_.begin();

    while (iter != listeners_.end())
    {
      if (iter->expired())
        iter = listeners_.erase(iter);
      else
      {
        luabind::call_function<void>(L, "pushEvent", iter->lock()->callFunction, dataTable);
        ++iter;
      }
    }

    dataTable = luabind::newtable(L);
    totalObjects_ = 0;
  }

  SCRIPT_LISTENER_PTR ScriptEvent::connect(luabind::object obj)
  {
    SCRIPT_LISTENER_PTR ev(new ScriptListener(this, obj));

    listeners_.push_back(ev);

    return ev;
  }

  void ScriptEvent::disconnectListener(ScriptListener * listener)
  {
    for (auto iter = listeners_.begin(); iter != listeners_.end(); iter++)
    {
      if (!iter->expired() && iter->lock().get() == listener)
      {
        listeners_.erase(iter);
        break;
      }
    }
  }

  ScriptRouter::ScriptRouter(lua_State * _L) :
    L(_L)
  {}

  void ScriptRouter::update()
  {
    auto iter = events_.begin();

    while (iter != events_.end())
    {
      if (iter->expired())
        iter = events_.erase(iter);
      else
      {
        iter->lock()->push();
        ++iter;
      }
    }
  }

  void ScriptRouter::registerEvent(SCRIPT_EVENT_PTR ev)
  {
    events_.push_back(ev);
  }
}
