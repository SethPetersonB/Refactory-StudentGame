// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/GameInstance.h"
#include "../include/Transform.h"
#include "../include/Physics.h"
#include "../include/sprite.h"
#include "../include/Input.h"
#include "../include/UIFrame.h"
#include "../include/GSM.h"
#include "../include/ClickDetector.h"
#include "../include/EnemyLogic.h"
#include "../include/object_stats.h"
#include "../include/StructureLogic.h"
#include "../include/Logger.h"
#include "../include/Particles.h"
#include "../include/Controller.h"
#include "../include/Animator.h"
#include "../include/ButtonMenu.h"
#include "../include/Waves.h"
#include "StructureBase.h"
//#include "Disappear.h"

#include <stdexcept>

using namespace Logger;

namespace Engine
{
  /****************************************************************************/
  /*!
    \brief
      Creates a new component of a given type. When you create a new component
      type, add it to this list and the object initializer list below.

    \param type
      Type of component that will be created

    \param owner
      GameInstace that will own the component

    \return
      Component pointer to the newly created component
  */
  /****************************************************************************/
  Component * ComponentHandler::createComponent(const std::string & type, GameInstance * owner)
  {
    if (type == "Transform")
      return createComponent<Transform>(owner);
    else if (type == "Physics")
      return createComponent<Physics>(owner);
    else if (type == "Collider")
      return createComponent<Collider>(owner);
    else if (type == "Sprite")
      return createComponent<Sprite>(owner);
    else if (type == "UIFrame")
      return createComponent<UIFrame>(owner);
    else if (type == "Controller")
      return createComponent<Controller>(owner);
    else if (type == "ClickDetector")
      return createComponent<ClickDetector>(owner);
    else if (type == "EnemyLogic")
      return createComponent<EnemyLogic>(owner);
    else if (type == "ObjectStats")
      return createComponent<ObjectStats>(owner);
    else if (type == "StructureLogic")
      return createComponent<StructureLogic>(owner);
    else if (type == "ParticleEmitter")
      return createComponent<ParticleEmitter>(owner);
    else if (type == "Animator")
      return createComponent<Animator>(owner);
    else if (type == "ButtonMenu")
      return createComponent<ButtonMenu>(owner);
    else if (type == "WaveController")
      return createComponent<WaveController>(owner);
    else if (type == "Tile")
      return createComponent<Tile>(owner);
    else if (type == "StructureBase")
      return createComponent<StructureBase>(owner);
    else
    {
      Log<Error>("Component type '%s' not recognized. Please add a creation field in Event_Connection.cpp", type.c_str());
      throw std::runtime_error("Component type not recognized, please add a creation field in Event_connection.cpp");
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Creates a new component of a given type and intializes it. When you 
      create a new component type, add it to this list and the basic list above.
      If you object cannot be initialized with an object type, it is OK to call
      the basic constructor here

    \param type
      Type of component that will be created

    \param owner
      GameInstace that will own the component

    \param obj
      Object archerype to initialize the component to

    \return
      Component pointer to the newly created component
  */
  /****************************************************************************/
  Component * ComponentHandler::createComponent(const std::string & type, GameInstance * owner,
    const ParsedObject & obj)
  {
    if (type == "Transform")
      return createComponent<Transform>(owner, obj);
    else if (type == "Physics")
      return createComponent<Physics>(owner);
    else if (type == "Collider")
      return createComponent<RectangleCollider>(owner);
    else if (type == "Sprite")
      return createComponent<Sprite>(owner, obj);
    else if (type == "Controller")
      return createComponent<Controller>(owner, obj);
    else if (type == "UIFrame")
      return createComponent<UIFrame>(owner, obj);
    else if (type == "EnemyLogic")
      return createComponent<EnemyLogic>(owner, obj);
    else if (type == "StructureLogic")
      return createComponent<StructureLogic>(owner, obj);
    else if (type == "ObjectStats")
      return createComponent<ObjectStats>(owner, obj);
    else if (type == "ClickDetector")
      return createComponent<ClickDetector>(owner, obj);
    else if (type == "ParticleEmitter")
      return createComponent<ParticleEmitter>(owner, obj);
    else if (type == "Animator")
      return createComponent<Animator>(owner, obj);
    else if (type == "ButtonMenu")
      return createComponent <ButtonMenu>(owner, obj);
    else if (type == "Particle")
      return createComponent<Particle>(owner, obj);
    //else if (type == "Disappear")
    //  return createComponent<Disappear>(owner, obj);
    else
    {
      Log<Error>("Component type '%s' not recognized. Please add a creation field in Event_Connection.cpp", type.c_str());
      throw std::runtime_error("Component type not recognized, please add a creation field in Event_connection.cpp");
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Creates a new component handler of a given type. When you create a new component
      type, add it's handler to this list

    \param type
      Type of component handler that will be created

    \param owner
      Stage that the handler exists on

    \return
      ComponentHandler pointer to the newly created handler
  */
  /****************************************************************************/
  ComponentHandler * ComponentHandler::CreateHandler(const std::string & type, Stage * owner)
  {
    if (type == "Transform")
      return CreateHandler<TransformHandler>(owner);
    else if (type == "Physics")
      return CreateHandler<PhysicsHandler>(owner);
    else if (type == "Collider" || type == "CircleCollider" || type == "RectangleCollider")
      return CreateHandler<ColliderHandler>(owner);
    else if (type == "Sprite")
      return CreateHandler<SpriteHandler>(owner);
    else if (type == "Controller")
      return CreateHandler<ControllerHandler>(owner);
    else if (type == "UIFrame")
      return CreateHandler<UIFrameHandler>(owner);
    else if (type == "ClickDetector")
      return CreateHandler<ClickDetectorHandler>(owner);
    else if (type == "EnemyLogic")
      return CreateHandler<EnemyLogicHandler>(owner);
    else if (type == "ObjectStats")
      return CreateHandler<ObjectStatsHandler>(owner);
    else if (type == "StructureLogic")
      return CreateHandler<StructureLogicHandler>(owner);
    else if (type == "ParticleEmitter")
      return CreateHandler<ParticleEmitterHandler>(owner);
    else if (type == "Animator")
      return CreateHandler<AnimatorHandler>(owner);
    else if (type == "ButtonMenu")
      return CreateHandler<ButtonMenuHandler>(owner);
    else if (type == "WaveController")
      return CreateHandler<WaveControllerHandler>(owner);
    else if (type == "Tile")
      return CreateHandler<TileHandler>(owner);
    else if (type == "Particle")
      return CreateHandler<ParticleHandler>(owner);
    else if (type == "StructureBase")
      return CreateHandler<StructureBaseHandler>(owner);

    else
    {
      Log<Error>("Handler type '%s' not recognized. Please add a creation field in Event_Connection.cpp", type.c_str());
      throw std::runtime_error("Handler type not recognized, please add a creation field in Event_connection.cpp");
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Generic template to create components from a given object

    \param owner
      Owner of the component

    \param obj
      Archetype to initialize the component from

    \return
      Pointer to the newly created component
  */
  /****************************************************************************/
  template<typename T>
  T * ComponentHandler::createComponent(GameInstance * owner,
                                        const ParsedObject & obj)
  {
    T * newComp = new T(owner, obj);

    // Check for behavior overrides
    std::string behv = obj.getComponentProperty<std::string>(newComp->getComponentType(), "behavior");

    if (behv.size() != 0)
      newComp->setBehaviorScript("scripts/" + behv);

    ConnectEvents(newComp);
    return newComp;
  }

  /****************************************************************************/
  /*!
    \brief
      Generic template to create components

    \param owner
      Owner of the component

    \return
      Pointer to the newly created component
  */
  /****************************************************************************/
  template<typename T>
  T * ComponentHandler::createComponent(GameInstance * owner)
  {
    T * newComp = new T(owner);
    ConnectEvents(newComp);
    return newComp;
  }

  /****************************************************************************/
  /*!
    \brief
      Generic template to create ComponentHandlers

    \param stage
      Owner of the ComponentHandler

    \return
      Pointer to the newly created ComponentHandler
  */
  /****************************************************************************/
  template<typename T>
  T * ComponentHandler::CreateHandler(Stage * stage)
  {
    T * handler = new T(stage);
    handler->getLuaRegisters();
    return handler;
  }
}

