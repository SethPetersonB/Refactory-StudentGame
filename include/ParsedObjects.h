// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <map>
#include <string>
#include <vector>
#include "json/json.h"
#include <exception>
#include <utility>
#include <memory>

namespace Engine
{
  int GenerateParsedObjects(const std::string & objectFile);
  void FlushParsedObjects();
  const Json::Value & DefaultJson();

  struct ParsedObject
  {
    ParsedObject(const std::string & name, const Json::Value & object);
    // Map of components that an archetype contains and their initial values

    const std::string name_;  // Name of the archetype

    bool hasComponentType(const std::string & type) const;

    const std::pair<std::string, Json::Value> & getComponent(const std::string & name) const;
    const std::vector<std::pair<std::string, Json::Value>> & getCompList() { return components; }
    const std::vector<std::string> & getScripts() const { return scripts_; }

    template<typename T>
    T getComponentProperty( const std::string & comp, 
                            const std::string & prop) const;

    // Static map of all currently parsed archetypes
    static std::map<std::string, std::unique_ptr<ParsedObject> > ObjectTypes;

    struct object_component_not_found : public std::exception
    {
      virtual const char * what() const throw();
    };

    struct object_property_not_found : public std::exception
    {
      virtual const char * what() const throw();
    };

    struct unsupported_property_type : public std::exception
    {
      virtual const char * what() const throw();
    };

  private:
    // Private and not implemented so that it cannot be coppied
    ParsedObject(const ParsedObject &) = delete;
    ParsedObject & operator=(const ParsedObject &) = delete;

    std::vector<std::pair<std::string, Json::Value>> components;
    std::vector<std::string> scripts_;

  };

  struct invalid_object_file : public std::exception
  {
    virtual const char * what() const throw();
  };

  struct invalid_object : public std::exception
  {
    virtual const char * what() const throw();
  };

  struct duplicate_object : public std::exception
  {
    virtual const char * what() const throw();
  };
}

