// Primary Author : Philip Nygard
//
// Note: Don't count towards code contribution.
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/ParsedObjects.h"

// Gets Json value assocated with the given name
Json::Value getProp(const Json::Value & prop, const std::string & name)
{
  const Json::Value & defValue = Engine::DefaultJson();

  Json::Value propertyValue = prop.get(name, defValue);

  if (propertyValue == defValue)
    return defValue;

  return propertyValue;
}

namespace Engine
{
  // All retrieve data from a property froom a given type. Only given types are valid

  // Ints
  template<>
  int ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop).asInt();
  }

  // Unsigned ints
  template<>
  unsigned ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop).asUInt();
  }

  // Floats
  template<>
  float ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop).asFloat();
  }

  // Doubles
  template<>
  double ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop).asDouble();
  }

  // Booleans
  template<>
  bool ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop).asBool();
  }

  // Strings
  template<>
  std::string ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop).asString();
  }

  // Json values
  template<>
  Json::Value ParsedObject::getComponentProperty(const std::string & comp,
    const std::string & prop) const
  {
    return getProp(getComponent(comp).second, prop);
  }

}
