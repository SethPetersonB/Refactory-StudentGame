// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <string>
#include <memory>

#include "Logger.h"

class Sandbox;

class Script
{
public:
  Script(int scriptIndex, std::string src);
  ~Script();

  Script(const Script &) = delete;
  Script & operator=(const Script &) = delete;

  void wait(double time = 0);
  void setDisabled(bool disabled);
  void timeTick(double tick);

  bool isWaiting() const;
  double getWaitTime() const;
  double getElapsedWaitTime() const;

  bool isDisabled() const;

  const int index;

  luabind::object job;
  luabind::object src;
  luabind::object env;

private:
  friend class Sandbox;

  std::string file_;
  bool disabled_;

  bool waiting_;
  double waitTime_;
  double elapsedTime_;

};

using SCRIPT_PTR = std::shared_ptr<Script>;

class Sandbox
{
public:
  Sandbox(const std::string & source = "scripts/sandbox.lua");
  ~Sandbox();

  SCRIPT_PTR loadScript(const std::string & source)
  {
    using namespace Logger;

    try
    {
      return luabind::call_function<SCRIPT_PTR>(state_, "loadScript", source);
    }
    catch (const std::exception & err)
    {
      Log<Error>(err.what());
      return SCRIPT_PTR(new Script(-1, source));
    }
  }

  template<typename T>
  SCRIPT_PTR loadScript(const std::string & source, T & parent)
  {
    using namespace Logger;

    try
    {
      return luabind::call_function<SCRIPT_PTR>(state_, "loadScript", source, parent);
    }
    catch (const std::exception & err)
    {
      Log<Error>(err.what());
      return SCRIPT_PTR(new Script(-1, source));
    }
  }

  void unloadScript(SCRIPT_PTR script);
  void start();

  lua_State * getLuaState() { return state_; }
  void update(float dt);

private:
  std::string source_;
  lua_State * state_;
};

