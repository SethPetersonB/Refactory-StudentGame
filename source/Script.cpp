// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Logger.h"
#include "../include/Script.h"
#include <fstream>

using namespace Logger;

// Script logging levels and functions
struct ScriptInfo : Info
{
  static const char * levelId() { return "SCRIPT INFO"; }
};

struct ScriptWarning : Warning
{
  static const char * levelId() { return "SCRIPT WARNING"; }
};

struct ScriptError : Error
{
  static const char * levelId() { return "SCRIPT ERROR"; }
};

template<typename T>
int lua_logger(const char *strng)
{
  return Log<T>(strng);
}


// Sandbox 
Sandbox::Sandbox(const std::string & source) :
  source_(source), state_(luaL_newstate())
{
  using namespace luabind;

  luaL_openlibs(state_);
  open(state_);
  
  module(state_)[
    namespace_("log")[
      def("info", &lua_logger<ScriptInfo>),
      def("warn", &lua_logger<ScriptWarning>),
      def("error", &lua_logger<ScriptError>)
    ], 
      class_<Script, SCRIPT_PTR>("Script")
        .def(constructor<int, std::string>())
        .def("Wait", &Script::wait)
        .def("IsWaiting", &Script::isWaiting)
        .def("ElapsedTime", &Script::getElapsedWaitTime)
        .def("TimeTick", &Script::timeTick)

        .def_readonly("waitTime", &Script::waitTime_)
        .def_readonly("index", &Script::index)

        .def_readwrite("source", &Script::src)
        .def_readwrite("job", &Script::job)
        .def_readwrite("disabled", &Script::disabled_)
        .def_readwrite("env", &Script::env)
  ];

}

Sandbox::~Sandbox()
{
  lua_close(state_);
}

void Sandbox::start()
{
  if (luaL_dofile(state_, source_.c_str()))
    Log<Error>(lua_tostring(state_, -1));
}

void Sandbox::update(float dt)
{
  luabind::call_function<void>(state_, "update", dt);
}

void Sandbox::unloadScript(SCRIPT_PTR script)
{
  luabind::call_function<void>(state_, "unloadScript", script.get());
}

Script::Script(int scriptIndex, std::string src) :
index(scriptIndex), file_(src), disabled_(false), 
waiting_(true), waitTime_(0), elapsedTime_(0)
{}

void Script::wait(double time)
{
  waiting_ = true;
  waitTime_ = time;
  elapsedTime_ = 0;
}

Script::~Script()
{
}
void Script::setDisabled(bool disabled)
{
  disabled_ = disabled;
}

bool Script::isWaiting() const
{
  return waiting_;
}

double Script::getWaitTime() const
{
  return waitTime_;
}

double Script::getElapsedWaitTime() const
{
  return elapsedTime_;
}

bool Script::isDisabled() const
{
  return disabled_;
}

void Script::timeTick(double tick)
{
  if ((elapsedTime_ += tick) >= waitTime_)
    waiting_ = false;

}
