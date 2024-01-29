// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/GameInstance.h"
#include <exception>
#include "../include/GSM.h"
#include <algorithm>
#include "../include/Logger.h"
#include "../include/Script.h"
#include "../include/ScriptSignal.h"

using namespace Logger;

namespace Engine
{
  const std::string GameInstance::DEF_SCRIPT_PATH = "scripts/";

  /****************************************************************************/
  /*!
    \brief
      Creates a new basic GameInstance

    \param stage
      Stage the stage the instance will be created on
  */
  /****************************************************************************/
  GameInstance::GameInstance( Stage * stage) : 
                              objectType_("CUSTOM"), objectId_(AssignId()), stage_(stage)
  {
    initHierarchy();
  }

  /****************************************************************************/
  /*!
    \brief
      Creates a new GameInstance from an object archetype

    \param stage
      Stage the stage the instance will be created on

    \param type
      Type of archetyp to create the object from
  */
  /****************************************************************************/
  GameInstance::GameInstance(Stage * stage, const std::string & type) :
    objectType_(type), objectId_(AssignId()), stage_(stage)
  {
    //GameInstanceList[objectId_] = this;

    try
    {
      ParsedObject & objectType = *ParsedObject::ObjectTypes.at(type);

      initHierarchy();

      for(auto & component_entry : objectType.getCompList())
      {
        if (component_entry.second != DefaultJson())
        {
          Component * comp = addComponent(component_entry.first, objectType);

          auto compProps = nullptr;
        }
        else
          addComponent(component_entry.first);
      }

    }
    catch (const std::out_of_range &)
    {
      Log<Error>("Invalid instance of type '%s'", type.c_str());
      throw std::runtime_error("Invalid instance of type " + type);
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Destructor for GameInstances. Removes it from the InstanceList of the 
      stage
  */
  /****************************************************************************/
  GameInstance::~GameInstance()
  {
    stage_->unloadHierarchyInstance(this);

    //GameInstanceList.erase(objectId_);
    // Destroy all components and free them from memory
    for (auto comp : components_)
    {
      delete comp;
    }

    for (auto & script : scripts_)
    {
      stage_->unloadScript(script);
    }
  }
  
  /****************************************************************************/
  /*!
    \brief
      Gets the archetype an object was created from 

    \return 
      string reference fo the object's archetyp ("CUSTOM") if no
      archerype was given on the objects creation
  */
  /****************************************************************************/
  const std::string & GameInstance::getObjectType() const
  {
    return objectType_;
  }

  /****************************************************************************/
  /*!
    \brief
      Assigns a unique ID to an object

    \return
      Unsigned long to use as the ID
  */
  /****************************************************************************/
  unsigned long GameInstance::AssignId()
  {
    static unsigned long allocated = 0;

    return allocated++;
  }

  /****************************************************************************/
  /*!
    \brief
      Object factory for GameInstances

    \param stage
      Stage to create the instance on

    \param type
      type to create the object from

    \return
      A pointer to the instance created
  */
  /****************************************************************************/
  GameInstance * GameInstance::New(Stage * stage, const std::string & type)
  {
    return new GameInstance(stage, type);
  }

  /****************************************************************************/
  /*!
    \brief
      Object factory for GameInstances. Creates an untyped object

    \param stage
      Stage to create the instance on

    \return
      A pointer to the instance created
  */
  /****************************************************************************/
  GameInstance * GameInstance::New(Stage * stage)
  {
    return new GameInstance(stage);
  }

  /****************************************************************************/
  /*!
    \brief
      Gets an object's unique objectID and returns it.

    \return
      the object's ID
  */
  /****************************************************************************/
  unsigned long GameInstance::getId() const
  {
    return objectId_;
  }

  /****************************************************************************/
  /*!
    \brief
      Adds a component of a given type to the GameInstance and registers it with
      the appropriate ComponentHandler if possible

    \param type
      Name of the type of componet to create

    \return
      A pointer to the component created
  */
  /****************************************************************************/
  Component * GameInstance::addComponent(const std::string & type)
  {
    Component * newComponent = stage_->createComponentFromType(this, type);

    components_.push_back(newComponent);

    hierarchy_[newComponent->getComponentType().c_str()] = newComponent;

    return newComponent;
  }

  /****************************************************************************/
  /*!
    \brief
      Adds a component of a given type to the GameInstance and registers it with
      the appropriate ComponentHandler if possible. Initializes objects 
      properties to those defined in an archetype

    \param type
      Name of the type of componet to create

    \param obj
      Archetype to create the object from

    \return
      A pointer to the component created
  */
  /****************************************************************************/
  Component * GameInstance::addComponent( const std::string & type, 
                                          const ParsedObject & obj)
  {
    Component * newComponent = stage_->createComponentFromType(this, type, obj);

    components_.push_back(newComponent);

    hierarchy_[newComponent->getComponentType().c_str()] = newComponent;
    return newComponent;
  }

  Messenger & GameInstance::getMessenger()
  { 
    return objMessenger_;
  }


  const Messenger & GameInstance::getMessenger() const
  {
    return objMessenger_;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the stage that an object exists on

    \return
      Poitner to the object's stage
  */
  /****************************************************************************/
  Stage *const GameInstance::getStage() const
  {
    return stage_;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets a pointer to a component on an object. Returns NULL if the given 
      component was not found

    \param comp
      The name of the component to find on the object
      
    \return
      A pointer to the desired component, or NULL if it was not found
  */
  /****************************************************************************/
  Component * GameInstance::getComponent(const std::string & comp) const
  {
    for(unsigned i = 0; i < components_.size(); i++)
    {
      if(components_[i]->getComponentType() == comp)
        return components_[i];
    }

    return nullptr;
  }

  /****************************************************************************/
  /*!
    \brief
      Checks each component in a GameInstance for it's dependencies and finds
      if they exist in the GameInstance. Returns a list of all missing 
      dependencies -- WARNING-- this is potentially a very expensive opperation,
      avoid checking dependencies as much as possible
      
    \return
      A pointer to the desired component, or NULL if it was not found
  */
  /****************************************************************************/
  std::vector<std::string> GameInstance::checkDependencies() const
  {
    std::vector<std::string> dependendencies;

    // Loop through component list
    for(auto & check_component : components_)
    {
      // Get component's dependencies
      std::vector<std::string> compDeps = check_component->getDependencies();

      for(auto & curr_dependencies : compDeps)
      {
        // If the GameInstance does not have a required component
        if(getComponent(curr_dependencies) == NULL)
        {
          for(unsigned long i = 0; i < dependendencies.size(); i++)
          {
            if(dependendencies[i] == curr_dependencies)
            {
              break;
            }
          }

          // If it was not found add it to the dependencies list
          dependendencies.push_back(curr_dependencies);
        }
      }
    }

    return dependendencies;
  }

  void GameInstance::loadScript(const std::string & script)
  {
    std::string path = script;

    if (path.compare(0, DEF_SCRIPT_PATH.length(), DEF_SCRIPT_PATH) != 0)
    {
      path = DEF_SCRIPT_PATH + path;
    }

    scripts_.push_back(stage_->loadScript(path, this));
  }

  void GameInstance::initScripts()
  {
    for (auto component : components_)
    {
      if (component->isBehaviorDefined())
        loadScript(component->getBehaviorScript());
    }

    try
    {
      ParsedObject & objectType = *ParsedObject::ObjectTypes.at(objectType_);

      for (auto script : objectType.getScripts())
        loadScript(script);
    }
    catch (std::out_of_range) {}

  }

  void GameInstance::registerScriptEvent(std::shared_ptr<ScriptEvent> ev)
  {
    events_.push_back(ev);
    hierarchy_[ev->eventName().c_str()] = ev;
  }

  void GameInstance::registerScriptEvent(std::vector<std::shared_ptr<ScriptEvent>> evs)
  {
    for(auto & ev : evs)
      registerScriptEvent(ev);
  }


  void GameInstance::initHierarchy()
  {
    // Init hierarcy as a table
    hierarchy_ = luabind::newtable(stage_->getScriptEventRouter().L);

    hierarchy_["name"] = objectType_;
    hierarchy_["id"] = objectId_;
    hierarchy_["_RAW"] = this;
  }

  luabind::object & GameInstance::getHierarchy()
  { 
    return hierarchy_; 
  }

  // Exceptions

  const char * instance_not_found::what() const throw()
  {
    return "No GameInstance found with the given ID";
  }

  const char * instance_invalid_type::what() const throw()
  {
    return "Attempted to create a GameInstance of an unknown type";
  }

  const char * ParsedObject::object_component_not_found::what() const throw()
  {
    return "Attempted to get a component that does not exist on an object";
  }

  const char * ParsedObject::object_property_not_found::what() const throw()
  {
    return "Attempted to get an unknown property from an object";
  }

  const char * ParsedObject::unsupported_property_type::what() const throw()
  {
    return "Cannot get an objects property of the desired type! Valid types are:\nInt\nUnsigned Int\nFloat\nDouble\nBoolean\nString\nJson Value";
  }
}
