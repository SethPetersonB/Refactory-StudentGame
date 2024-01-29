// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/StructureLogic.h"
#include "../include/GraphicsPipeline.h"
#include "../include/audio_startup.h"
//#include "../include/StructureCombat.h" // Possible future implementation.
#include "../include/GSM.h"
#include "../include/EnemyLogic.h"

namespace Engine
{

  /****************************************************************************/
  /*!
    \brief
      Constructor for the StructureLogic handler.

    \param stage
      Pointer to the stage owner of the handler
  */
  /****************************************************************************/
  StructureLogicHandler::StructureLogicHandler(Stage* stage) : ComponentHandler( stage, "StructureLogic" )
  {
    dependencies_ = { "object_stats" };
  }

  //StructureLogicHandler::~StructureLogicHandler() {};

  /****************************************************************************/
  /*!
    \brief
      Update function for StructureLogicHandler. Currently doesn't do anything
  */
  /****************************************************************************/
  void StructureLogicHandler::update()
  {
    /* Currently not needed. */
    /*
    for (unsigned long i = 0; i < componentList_.size(); ++i)
    {
      if (componentList_[i]->getComponentType() == "StructureLogic")
      {
        StructureLogic * Structure = dynamic_cast<StructureLogic *>(componentList_[i]);
      }
    }
    */
  }

  /****************************************************************************/
  /*!
    \brief
      Event connections for StructureLogic components. Conencts events for
      setting and retrieving various flags on the component and 
      collision events

    \param base_sub
      Pointer to the Component base class of the object to connect to
  */
  /****************************************************************************/
  void StructureLogicHandler::ConnectEvents(Component * base_sub)
  {
    StructureLogic * sub = dynamic_cast<StructureLogic *>(base_sub);

    using namespace std::placeholders;

    Messenger & objMessenger = sub->getParent().getMessenger();
    
    SUBSCRIBER_ACTION onHealthFlagChange = std::bind(&StructureLogic::OnStructureHealthFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onCollisionStartedFlagChange = std::bind(&StructureLogic::OnStructureCollisionFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onAttackFlagChange = std::bind(&StructureLogic::OnStructureAttackFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onStructureCollisionStarted = std::bind(&StructureLogic::onCollisionStarted, sub, std::placeholders::_1);

    REQUEST_ACTION healthflagRequest = std::bind(&StructureLogicHandler::StructureHealthFlagRequest, sub, std::placeholders::_1);
    REQUEST_ACTION collisionflagRequest = std::bind(&StructureLogicHandler::StructureCollisionFlagRequest, sub, std::placeholders::_1);
    REQUEST_ACTION attackflagRequest = std::bind(&StructureLogicHandler::StructureAttackFlagRequest, sub, std::placeholders::_1);

    objMessenger.Subscribe(objMessenger, "HealthFlagSet", onHealthFlagChange);
    objMessenger.Subscribe(objMessenger, "CollisionFlagSet", onCollisionStartedFlagChange);
    objMessenger.Subscribe(objMessenger, "AttackFlagSet", onAttackFlagChange);

    objMessenger.Subscribe(objMessenger, "CollisionStarted", onStructureCollisionStarted);

    
    objMessenger.SetupRequest("EnemyHealthFlag", healthflagRequest);
    objMessenger.SetupRequest("EnemyCollisionFlag", collisionflagRequest);
    objMessenger.SetupRequest("EnemyAttackFlag", attackflagRequest);
  }

  void StructureLogicHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<StructureLogic, Component>("StructureLogic")
      .property("shield", &StructureLogic::GetShield, &StructureLogic::SetShield)
      .property("timesHit", &StructureLogic::GetTimesHit, &StructureLogic::SetTimesHit)
      .property("immuneTurn", &StructureLogic::GetImmuneTurn, &StructureLogic::SetImmuneTurn)
      );
  }

  // Request event for health flag
  void StructureLogicHandler::StructureHealthFlagRequest(const StructureLogic* member, Packet& data)
  {
    dynamic_cast<Message<unsigned> &>(data).data = member->GetStructureHealthFlag();
  }

  // Request event for attack flag
  void StructureLogicHandler::StructureAttackFlagRequest(const StructureLogic* member, Packet& data)
  {
    dynamic_cast<Message<unsigned> &>(data).data = member->GetStructureAttackFlag();
  }

  // Request event for collision flag
  void StructureLogicHandler::StructureCollisionFlagRequest(const StructureLogic* member, Packet& data)
  {
    dynamic_cast<Message<unsigned> &>(data).data = member->GetStructureCollisionFlag();
  }

  /****************************************************************************/
  /*!
    \brief
      Basic constructor for StructureLogic components

    \param owner
      Pointer to the owner of the component
  */
  /****************************************************************************/
  StructureLogic::StructureLogic(GameInstance* owner) :
                  Component( owner, "StructureLogic")
  {
  }

  /****************************************************************************/
  /*!
    \brief
      Archetype constructor for the StructureLogic component. Initializes 
      values to those found in the given archetype

    \param owner
      Pointer to the owner of the component

    \param obj
      Reference to the ParsedObject archetype to construct the component
      with
  */
  /****************************************************************************/
  StructureLogic::StructureLogic(GameInstance * owner, const ParsedObject & obj) :
                  Component( owner, "StructureLogic")
  {
    StructureAttackFlag = obj.getComponentProperty<unsigned>("StructureLogic", "StructureAttackFlag");
    StructureHealthFlag = obj.getComponentProperty<unsigned>("StructureLogic", "StructureHealthFlag");
    StructureCollisionFlag = obj.getComponentProperty<unsigned>("StructureLogic", "StructureCollisionFlag");

  }

/*******************************************************************************************/
  // Message system operations -- takes packet info and connects functions (Ex: to perform combat. )
/*******************************************************************************************/
  /* THIS IS LARGELY UNFINISHED! */

  // Collision started event
  void StructureLogic::onCollisionStarted(const Packet& payload)
  {
    /* Do nothing if our collision is not valid */
    if ( StructureCollisionFlag & STRUCTURECOLLIDENULL )
    {
      return;
    }
    const unsigned othercol = dynamic_cast<const Message<unsigned long> &>(payload).data;

    try
    {
      GameInstance& other = getParent().getStage()->getInstanceFromID(othercol);

    /* Do nothing when colliding with self or other buildings... for now */
    const StructureLogic* safe = dynamic_cast<StructureLogic*>(other.getComponent("StructureLogic"));
    if ( safe )
    {
        return;
    }

    GameInstance& us = getParent();
    int OurHp = us.RequestData<int>("HP");
    int MaxHp = us.RequestData<int>("MaxHP");

    const ObjectStats* objother = dynamic_cast<ObjectStats*>(other.getComponent("ObjectStats"));

    // Do nothing if it has no stats.
    if ( !objother )
    {
      return;
    }

    /* Only collide with enemies with valid collision */
    const EnemyLogic* enemy = dynamic_cast<EnemyLogic*>(other.getComponent("EnemyLogic"));
    if ( ! enemy )
    {
      return;
    }
    /*
    if ( enemy )
    {
      if (enemy->GetEnemyCollisionFlag() & ENEMYCOLLIDENULL)
      {
        return;
      }
    }
    */

    unsigned OtherStats = objother->GetStatFlag();

    /* Combat portion of collision */
    // THIS IS STRUCTURES TAKING DAMAGE
    
    // we only want to decrement health from the bottom block
    if (0 == (int)getParent().RequestData<float>("TransformDepth"))
    {
      if (OtherStats & DAMAGE)
      {
        ++timesHit_;
        int dmg = objother->GetDamage() - shield_;

        if (immuneTurn_ > 0)
          if (timesHit_ % immuneTurn_ == 0)
            dmg = 0; // take no damage

        using namespace Logger;
        OurHp -= dmg;
        float damPerc = static_cast<float>(OurHp) / static_cast<float>(MaxHp);

        Audio_Engine* AEngine = GetAudioEngine();
        AEngine->PlaySounds("enemy_melee.wav", Vector3(), 0.05f);
        getParent().PostMessage("HpSet", Message<int>(OurHp));
        getParent().PostMessage("CreateParticles", Message<std::string>("spiral"));

        // Set color based on HP
        DrawToken item = getParent().RequestData<DrawToken>("Graphic");
        glm::vec4 color = item.getShade();
        item.setShade(glm::vec4{ color.r, damPerc, damPerc, color.a });
      }
    }

    //CollisionInteraction( us, other); // Save this for later.

    if ( OurHp <= 0 )
    {
      Die();
    }
    }
    catch (std::out_of_range &)
    {}
  }

  // Attack flag changed event
  void StructureLogic::OnStructureAttackFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetStructureAttackFlag( newstats );
  }

  // Collision flag changed event
  void StructureLogic::OnStructureCollisionFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetStructureCollisionFlag( newstats );
  }

  // Health flag changed event
  void StructureLogic::OnStructureHealthFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetStructureHealthFlag( newstats );
  }
/*******************************************************************************************/
  // Getter/Setter functions, "on______" is used by messaging system.
/*******************************************************************************************/
  /* Sets the grid value to empty, turns ObjectStats Flag off, turns collider off. */
  /* TODO: Add a value that triggers a death animation for towers. */
  /****************************************************************************/
  /*!
    \brief
      Sets a structure to logically dead by turing off it's flags
  */
  /****************************************************************************/
  void StructureLogic::Die()
  {
    //unsigned atkflg = GetStructureAttackFlag() | STRUCTUREATTACKNULL;
    //unsigned colflg = GetStructureCollisionFlag() | STRUCTURECOLLIDENULL;
    //unsigned hpflg = GetStructureHealthFlag() | STRUCTUREHEALTHNULL;
    //SetStructureAttackFlag( atkflg );
    //SetStructureCollisionFlag( colflg );
    //SetStructureHealthFlag( hpflg );

    /* Turn off the stat flag for assurance, I guess? */
    getParent().PostMessage("StatFlagSet", Message<unsigned>(0));

    Audio_Engine* AEngine = GetAudioEngine();
    AEngine->PlaySounds("ktttt.wav", Vector3(), -20.0f);

    /* Inform the grid that the structure has been destroyed. */
    //glm::vec2 gridpos = getParent().RequestData<glm::vec2>("TilePos");
    //Grid& grid = getParent().getStage()->GetGrid();
    //Stage* condemnation = getParent().getStage();
    //grid.RemoveColumn(int(gridpos.x), int(gridpos.y));
    //GameInstance* pc = condemnation->getMessenger().Request<GameInstance*>("PlayerController");
    //pc->PostMessage("DeleteWall", Message<GameInstance&>(getParent())); // delete wall
    if (getParent().getObjectType() == "Wall")
    {
      getParent().PostMessage("PopTile", Message<bool>(true));
    }
    else
    {
      getParent().getStage()->removeGameInstance(getParent());
    }

  }


  /****************************************************************************/
  /*!
    \brief
      Sets a structure to logically alive by turning on it's flags
  */
  /****************************************************************************/
  void StructureLogic::Live()
  {
    GameInstance& par = getParent();

    unsigned atkflg = GetStructureAttackFlag() & ~STRUCTUREATTACKNULL;
    unsigned colflg = GetStructureCollisionFlag() & ~STRUCTURECOLLIDENULL;
    unsigned hpflg = GetStructureHealthFlag() & ~STRUCTUREHEALTHNULL;
    SetStructureAttackFlag( atkflg );
    SetStructureCollisionFlag( colflg );
    SetStructureHealthFlag( hpflg );

    par.PostMessage("StatFlagSet", Message<unsigned>(3));
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the attack flag of the component

    \return
      The object's attack flag
  */
  /****************************************************************************/
  unsigned StructureLogic::GetStructureAttackFlag() const
  {
    return StructureAttackFlag;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the collision flag of the component

    \return
      The object's collision flag
  */
  /****************************************************************************/
  unsigned StructureLogic::GetStructureCollisionFlag() const
  {
    return StructureCollisionFlag;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the health flag of the component

    \return
      The object's health flag
  */
  /****************************************************************************/
  unsigned StructureLogic::GetStructureHealthFlag() const
  {
    return StructureHealthFlag;
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the object's attack flag to the given value

    \param val
      Value to set the flag to
  */
  /****************************************************************************/
  void StructureLogic::SetStructureAttackFlag(unsigned val)
  {
    StructureAttackFlag = val;
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the object's collision flag to the given value

    \param val
      Value to set the flag to
  */
  /****************************************************************************/
  void StructureLogic::SetStructureCollisionFlag(unsigned val)
  {
    StructureCollisionFlag = val;
  }


  /****************************************************************************/
  /*!
    \brief
      Sets the object's health flag to the given value

    \param val
      Value to set the flag to
  */
  /****************************************************************************/
  void StructureLogic::SetStructureHealthFlag(unsigned val)
  {
    StructureHealthFlag = val;
  }

}