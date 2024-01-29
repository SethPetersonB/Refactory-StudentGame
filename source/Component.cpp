// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/GameInstance.h"
#include "../include/GSM.h"
#include "../include/Logger.h"
#include <fstream>

using namespace Logger;

namespace Engine
{

  /****************************************************************************/
  /*!
    \brief
      Constructor for components. Set's the component's owner

    \param owner
      Owner of the component

    \param type 
      Type of component that is being created
  */
  /****************************************************************************/
  Component::Component( GameInstance * owner,
                        const std::string & type) : 
                        parent(owner), componentType_(type)
  {
    useDefaultBehavior();
    ScriptRouter & router = owner->getStage()->getScriptEventRouter();
    Messenger & messenger = owner->getMessenger();

    // Component script update event
    owner->registerScriptEvent(router.newEvent<float>(messenger, componentType_ + "Update"));
    owner->registerScriptEvent(router.newEvent<float>(messenger, componentType_ + "PreUpdate"));

    registerMe();
  }

  Component::~Component()
  {
    deregisterMe();
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the type of the component and returns it as a string

    \return 
      The pointer type's identifier
  */
  /****************************************************************************/
  const std::string & Component::getComponentType() const
  {
    return componentType_;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the lis of dependencies required by the component type

    \return 
      A const reference to the list of dependencies
  */
  /****************************************************************************/
  const std::vector<std::string> & Component::getDependencies() const
  {
    return getHandler()->getDependencies();
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the component handler associated with the component's type

    \return 
      A pointer to the handler of the component's type
  */
  /****************************************************************************/
  const ComponentHandler * Component::getHandler() const
  {
    return parent->getStage()->getHandler(getComponentType());
  }

  GameInstance & Component::getParent() const
  {
    return *parent;
  }

  /****************************************************************************/
  /*!
    \brief
      Registers a component with it's associated handler

    \return 
      A pointer to the handler of the component's type
  */
  /****************************************************************************/
  ComponentHandler * Component::registerMe()
  {
    ComponentHandler * handler;

    handler = parent->getStage()->getHandler(getComponentType());

    handler->registerComponent(this);

    return handler;
  }

  /****************************************************************************/
  /*!
    \brief
      Deregisters a comopnent from it's associated handler, if it exeists
  */
  /****************************************************************************/
  void Component::deregisterMe()
  {
    try
    {
      // Gets the component's handler and calls the handler's register function
      ComponentHandler * handler;

      handler = parent->getStage()->getHandler(getComponentType());

      handler->deregisterComponent(this);
    }
    // Component doesn't need to be deregistered if it's handler does not exist
    catch(component_handler_not_found)
    {}
  }

  void Component::setBehaviorScript(const std::string & behavior) 
  { 
    std::ifstream file(behavior);

    behaviorScript_ = behavior;
    behaviorDefined_ = file.good();

#ifdef _DEBUG
    //if(!behaviorDefined_)
      //Log<Warning>("Could not open behavior definition script '%s'", behavior.c_str());
#endif
  }

  void Component::useDefaultBehavior()
  {
    std::string fileName("scripts/Components/");
    fileName += componentType_ + ".lua";
    setBehaviorScript(fileName);
  }

  /****************************************************************************/
  /*!
    \brief
      Constructor for the ComponentHandler base class. Takes a string of the
      component handler's type, and adds it to the list of handlers. Fails and 
      throws a component_handler_duplication exception if another handler of the
      same type already exists
    
    \param type
      Component type associated with the handler, and key for the handler list

  */
  /****************************************************************************/
  ComponentHandler::ComponentHandler( Stage * owner, const std::string & type, bool pausable) :
                                      stage_(owner), handlerType_(type), isPausable_(pausable)
  {
    stage_->addHandler(this);
  }

  /****************************************************************************/
  /*!
    \brief
      Destructor for ComponentHandlers. Removes the handler from the handlers_ 
      list
  */
  /****************************************************************************/
  ComponentHandler::~ComponentHandler()
  {}

  bool ComponentHandler::IsValidComponent(Component * comp)
  {
    for (auto & check : componentList_)
    {
      if (check == comp)
        return true;
    }
    return false;
  }
  /****************************************************************************/
  /*!
    \brief
      Registers a given component with the handler by adding it to it's 
      component list. Throws a component_register_failed exception if it was 
      unable to add the component to the list, or a 
      component_register_type_mistmatch exception if attempting to rgister a 
      component with a non-matching handler 

    \exception component_register_failed
      Fired if the component failed to register with the handler 

    \exception component_register_type_mistmatch
      Fired if attempting to register a component witha mistmatched handler
  */
  /****************************************************************************/
  void ComponentHandler::registerComponent(Component * registar)
  {
    if(registar->getComponentType() == getComponentHandlerType())
    {
      try
      {
        // Add the componnt to the list
        componentList_.push_back(registar);
      }
      // Not enough space to register a new component (highly unlikely)
      catch (const std::bad_alloc &)
      {
        Log<Error>("Failed to register component '%s' on stage %s (stage order %d)", 
          getComponentHandlerType().c_str(), 
          getStage()->getStageName().c_str(), 
          getStage()->getStageOrder());
        throw std::runtime_error("Failed to create component");
      }
    }
    else
    {
      Log<Error>("Attempted to register a component of type '%s' on a handler of type '%s'",
        registar->getComponentType().c_str(),
        getComponentHandlerType().c_str());
      throw component_register_type_mistmatch();
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Deregisters a component from the component handler by removing it from 
      it's component list. Returns trueif the given component was found, false 
      if it was not

    \return 
      success code of the deregister. True if the component was found and 
      removed from the list, false if the component was not found on the list
  */
  /****************************************************************************/
  bool ComponentHandler::deregisterComponent(Component * registar)
  {
    for(unsigned i = 0; i < componentList_.size(); i++)
    {
      if(componentList_[i] == registar)
      {
         std::iter_swap(componentList_.begin() + i, componentList_.end() - 1);

         componentList_.pop_back();

         return true; // Component was found in the handler
      }
    }

    return false; // Component was not found in the handler
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the dependenceis of the component handler by returning a constant
      reference to its dependency list

    \return 
      constant reference to the handler's dependency list
  */
  /****************************************************************************/
  const std::vector<std::string> & ComponentHandler::getDependencies() const
  {
    return dependencies_;
  }

  /****************************************************************************/
  /*!
    \brief
      Returns a string naming the type of component the handler is responsible
      for

    \return 
      Constant string reference to the handler's type
  */
  /****************************************************************************/
  const std::string & ComponentHandler::getComponentHandlerType() const
  {
    return handlerType_;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the stage that a handler exists on

    \return
      A pointer to the stage
  */
  /****************************************************************************/
  Stage * ComponentHandler::getStage() const
  {
    return stage_;
  }

  void ComponentHandler::getLuaRegisters()
  {
    Log<Warning>("Lua registers not set up for component of type %s", handlerType_.c_str());
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the type of componet a handler handles

    \return
      String of the type of the handler
  */
  /****************************************************************************/
  const std::string &  ComponentHandler::getType() const
  {
    return handlerType_;
  }

  void ComponentHandler::tryLoadBehavior(Component * comp)
  {
    if(comp->isBehaviorDefined())
      comp->getParent().loadScript(comp->getBehaviorScript());
  }

  void ComponentHandler::updateComponents()
  {
    for (auto * component : componentList_)
      component->getParent().PostMessage(handlerType_ + "PreUpdate", GSM::get().getDisplay().GetFrameTime());

    // Calls C++ update function
    update();

    // Fires Component update, can be recieved by scripts
    for (auto * component : componentList_)
      component->getParent().PostMessage(handlerType_ + "Update", GSM::get().getDisplay().GetFrameTime());
  }
  // Exceptions

  const char * component_handler_duplication::what() const throw()
  {
    return "Attempted to duplicate a component handler on a stage!";
  }

  const char * component_register_type_mistmatch::what() const throw()
  {
    return "Component provided does not match the type of the handler it registered to!";
  }

  const char * component_handler_not_found::what() const throw()
  {
    return "Could not find the requested handler on its stage";
  }
}