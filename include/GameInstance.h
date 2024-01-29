// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <vector>
//#include <map>
#include <list>
#include <exception>
#include <string>
#include <luabind/luabind.hpp>

#include "ParsedObjects.h"
#include "Messages.h"
#include <memory>

class Script;

namespace Engine
{
  class Stage;  // Forward declaration of stage class  used in GSM.h

  class ScriptEvent;

  class Component;        // Forward declaration of Component class
  class ComponentHandler; // Forward declaration of ComponentHandler class


  class GameInstance
  {
  public:

    static const std::string DEF_SCRIPT_PATH;

    virtual ~GameInstance();
    unsigned long getId() const;
    
    Stage * const getStage() const;
    Component * getComponent(const std::string & comp) const;
    const std::vector<Component *> & getComponentList() const { return components_; }

    Component * addComponent(const std::string & type);
    Component * addComponent(const std::string & type, 
                            const ParsedObject & obj);

    Messenger & getMessenger();

    template<typename T>
    T RequestData(const std::string & type)
    {
      return objMessenger_.Request<T>(type);
    }


    template<typename T>
    void PostMessage(const std::string & eventType, const T & payload)
    {
      objMessenger_.Post(eventType, payload);
    }

    const Messenger & getMessenger() const;

    std::vector<std::string> checkDependencies() const;


    const std::string & getObjectType() const;
    
    void loadScript(const std::string & script);

    void initScripts();

    // Wrapper for most common type of script event registration.
    // Creates a new event on the object that is listening to it's messenger
    template<typename T>
    void newScriptEvent(const std::string & name)
    {
      registerScriptEvent(stage_->getScriptEventRouter().newEvent<T>(objMessenger_, name)), 
    }

    void registerScriptEvent(std::shared_ptr<ScriptEvent> ev);
    void registerScriptEvent(std::vector<std::shared_ptr<ScriptEvent>> evs);

    const std::vector<std::shared_ptr<ScriptEvent>> & getScriptEvents() { return events_; }

    static GameInstance * New(Stage * stage, const std::string & type);
    static GameInstance * New(Stage * stage);

    luabind::object & getHierarchy();

  private:

    void initHierarchy();

    // Private and implemented so only the factory can create them
    GameInstance(Stage * stage);
    GameInstance(Stage * stage, const std::string & objectType);

    // Private and not implemented so that it cannot be coppied
    GameInstance(const GameInstance &) = delete;
    GameInstance & operator=(const GameInstance &) = delete;

    static unsigned long AssignId();

    Messenger objMessenger_;
    const std::string objectType_;
    
    std::vector<Component *> components_; // List of components the object has
    std::vector<std::shared_ptr<Script>> scripts_;     // List of scripts the object owns
    std::vector<std::shared_ptr<ScriptEvent>> events_;   // List of registered events on the object
    // Object's unique ID
    // Represents it's positon in the GameInstanceList Vector
    const unsigned long objectId_;
    Stage * stage_;

    luabind::object hierarchy_;
  };

  class Component
  {
  public:

    Component(GameInstance * owner, const std::string & type);
    Component(const Component &) = delete;
    Component & operator=(const Component &) = delete;
    virtual ~Component() = 0;

    const std::string & getComponentType() const;
    const std::vector<std::string> & getDependencies() const;
    const ComponentHandler * getHandler() const;
    GameInstance & getParent() const;

    bool isBehaviorDefined() const { return behaviorDefined_; }
    void useDefaultBehavior();
    void setBehaviorScript(const std::string & behavior);

    std::string getBehaviorScript() const { return behaviorScript_; }

  private:
    // Private and not implemented so that it cannot be coppied

    bool behaviorDefined_;
    GameInstance * parent;  // Parent of the component
    ComponentHandler * registerMe();
    void deregisterMe(); 
    const std::string componentType_;
    std::string behaviorScript_;

  };

  class ComponentHandler
  {
  public:
    ComponentHandler(Stage * owner, const std::string & type, bool pausable = true);
    virtual ~ComponentHandler() = 0;

    void registerComponent(Component * registar);
    bool deregisterComponent(Component * registar);
    const std::vector<std::string> & getDependencies() const;
    //virtual Component * createComponent(GameInstance * owner) = 0;
    //virtual Component * createComponent(GameInstance * owner, 
                                        //const ParsedObject & obj) = 0;

    
    Component * createComponent(const std::string & type, GameInstance * owner);
    Component * createComponent(const std::string & type, GameInstance * owner,
                                const ParsedObject & obj);
    static ComponentHandler * CreateHandler(const std::string & type, Stage * owner);


    template<typename T>
    T * createComponent(GameInstance * owner);

    template<typename T>
    T * createComponent(GameInstance * owner,
                const ParsedObject & obj);

    template<typename T>
    static T * CreateHandler(Stage * stage);

    virtual void update() = 0;
    
    // Calls component update, then fires script updates
    void updateComponents();

    void tryLoadBehavior(Component * comp);

    virtual void getLuaRegisters();

    Stage * getStage() const;
    
    bool isPausable() { return isPausable_; }

    const std::string & getType() const;
  protected:
    // Private and not implemented so that it cannot be coppied
    ComponentHandler(const ComponentHandler &) = delete;
    ComponentHandler & operator=(const ComponentHandler &) = delete;

    bool IsValidComponent(Component * check);

    virtual void ConnectEvents(Component * sub) = 0;

    std::vector<Component *> componentList_;
    const std::string & getComponentHandlerType() const;
    std::vector<std::string> dependencies_;

    Stage * stage_;

  private:
    const std::string handlerType_;
    bool isPausable_;
  };

  struct instance_not_found : public std::exception
  {
    virtual const char * what() const throw();
  };

  struct instance_invalid_type : public std::exception
  {
    virtual const char * what() const throw();
  };

  struct component_handler_duplication : public std::exception
  {
  public:
    virtual const char * what() const throw();
  };

  struct component_handler_not_found : public std::exception
  {
  public:
    virtual const char * what() const throw();
  };

  struct component_register_failed : public std::exception
  {
  public:
    virtual const char * what() const throw();
  };

  struct component_register_type_mistmatch : public std::exception
  {
    virtual const char * what() const throw();
  };
}
