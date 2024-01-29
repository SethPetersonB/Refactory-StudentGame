// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/ParsedObjects.h"
#include <fstream>
#include "../include/GameInstance.h"
#include <algorithm>
#include "../include/Logger.h"

using namespace Logger;

namespace Engine
{
  // Map of all parsed object archetypes
  std::map < std::string, std::unique_ptr<ParsedObject> > ParsedObject::ObjectTypes;

  const Json::Value & DefaultJson()
  {
    static const Json::Value defValue;

    return defValue;
  }

  /****************************************************************************/
  /*!
    \brief
      Generates ParsedObjects from given Json file

    \param objectFile
      Path to the Json file containing object information

    \return 
      number of objects parsed and created
  */
  /****************************************************************************/
  int GenerateParsedObjects(const std::string & objectFile)
  {
    int count = 0;
    std::ifstream inputFile(objectFile.c_str());
    Json::Reader reader;
    Json::Value root;
    const Json::Value & defValue = DefaultJson();

    if(inputFile.is_open())
    {
      reader.parse(inputFile, root);

      std::vector<std::string> objectNames = root.getMemberNames();

      for(unsigned i = 0; i < objectNames.size(); i++)
      {
        Json::Value object = root.get(objectNames[i], defValue);

        if(object != defValue)
        {
          try
          {
            ParsedObject::ObjectTypes[objectNames[i]] = std::unique_ptr<ParsedObject>(new ParsedObject(objectNames[i], object));
            ++count;
          }
          catch (duplicate_object &)
          {
            Log<Warning>("Attempted to create duplicate object of type '%s'. It has been ignored", 
              objectNames[i].c_str());
          }
        }
        else
          Log<Error>("Read error while parsing objects!");
      }
    }
    else
    {
      Log<Error>("Failed to open the object file '%s'", objectFile.c_str());
      throw invalid_object_file();
    }

    return count;
  }

  /****************************************************************************/
  /*!
    \brief
      Flushes all ParsedObjects from memory and empties the ObjectTypes list
  */
  /****************************************************************************/
  void FlushParsedObjects()
  {
    ParsedObject::ObjectTypes.clear();
  }

  /****************************************************************************/
  /*!
    \brief
      Constructor for ParsedObjects. Creates an object with the given name from 
      the given Json values

    \param name 
      Name of the object type being created

    \param object 
      Value list of the object in Json format
  */
  /****************************************************************************/
  ParsedObject::ParsedObject( const std::string & name, 
                              const Json::Value & object) : name_(name)
  {
    const Json::Value & defValue = DefaultJson();
    Json::Value property = object.get("components", defValue);
    Json::Value scripts = object.get("scripts", defValue);

    // Object's component list is not properly deffined
    if ((property == defValue || !property.isArray()) || (scripts != defValue && !scripts.isArray()))
    {
      Log<Error>("Object of type '%s' is malformed", name);
      throw invalid_object();
    }

    for(unsigned i = 0; i < property.size(); i++)
    {
      // Get component initial values
      std::string propName = property[i].asCString();
      Json::Value initValues = object.get("component." + propName, defValue);
      components.push_back(std::make_pair(property[i].asCString(), initValues));
      //components[property[i].asCString()] = initValues;
    }

    if (scripts != defValue)
    {
      for (unsigned i = 0; i < scripts.size(); i++)
      {
        // Get component initial values
        std::string scrName = scripts[i].asCString();
        scripts_.push_back(scrName);
        //components[property[i].asCString()] = initValues;
      }
    }
    //addDependencies();
  }

  /****************************************************************************/
  /*!
    \brief
      Gets json value associated with a component type from a ParsedObject

    \param name
      Name of the component to get

    \return
      A pair containing the name of the componet and the json value 
      associated with it
  */
  /****************************************************************************/
  const std::pair<std::string, Json::Value> & ParsedObject::getComponent(const std::string & name) const
  {
    for (auto & comp_entry : components)
    {
      if (comp_entry.first == name)
        return comp_entry;
    }

    Log<Error>("Component of type '%s' not found", name.c_str());
    throw object_component_not_found();
  }

  /****************************************************************************/
  /*!
    \brief
      Checks if a ParsedObject has the given component type in it's list

    \return 
      True if the component was found, false if it wasn't
  */
  /****************************************************************************/
  bool ParsedObject::hasComponentType(const std::string & type) const
  {
    for(auto & component_entry : components)
    {
      if(component_entry.first == type)
        return true;
    }

    return false;
  }

  // Exceptions

  const char * invalid_object_file::what() const throw()
  {
    return "Failed to open given object file";
  }

  const char * invalid_object::what() const throw()
  {
    return "Attempted to create an object type with invalid data";
  }

  const char * duplicate_object::what() const throw()
  {
    return "Attempted to create a duplicate object type";
  }

}

