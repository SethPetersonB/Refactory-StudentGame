// Primary Author : Seth Peterson
//
// � Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright � Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/EnemyLogic.h"
#include "../include/object_stats.h"
#include "../include/Physics.h"
#include "../include/GraphicsPipeline.h"
#include "../include/audio_startup.h"
#include "../include/EnemyCombat.h"
#include "../include/EnemyPathing.h"
#include "../include/GSM.h"
#include "../include/StructureLogic.h"
#include "temp_utils.hpp"

namespace Engine
{
  /* Constructor for EnemyLogicHandler */
  EnemyLogicHandler::EnemyLogicHandler(Stage* stage) : ComponentHandler( stage, "EnemyLogic" )
  {
  }

  /* Updates all of the EnemyLogic components in the stage */
  void EnemyLogicHandler::update()
  {
    for (unsigned long i = 0; i < componentList_.size(); ++i)
    {
      if (componentList_[i]->getComponentType() == "EnemyLogic")
      {
        EnemyLogic * Enemy = dynamic_cast<EnemyLogic *>(componentList_[i]);
        Pathing( *Enemy );
      }
    }
  }

  /*
  Message system integration.
  */
  void EnemyLogicHandler::ConnectEvents(Component * base_sub)
  {
    EnemyLogic * sub = dynamic_cast<EnemyLogic *>(base_sub);

    using namespace std::placeholders;

    Messenger & objMessenger = sub->getParent().getMessenger();
    
    SUBSCRIBER_ACTION onHealthFlagChange = std::bind(&EnemyLogic::OnEnemyHealthFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onCollisionFlagChange = std::bind(&EnemyLogic::OnEnemyCollisionFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onAttackFlagChange = std::bind(&EnemyLogic::OnEnemyAttackFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onPathingFlagChange = std::bind(&EnemyLogic::OnEnemyPathingFlagChange, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onEnemyCollisionStarted = std::bind(&EnemyLogic::onCollisionStarted, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onEnemyCollisionEnded = std::bind(&EnemyLogic::onCollisionEnded, sub, std::placeholders::_1);

    REQUEST_ACTION healthflagRequest = std::bind(&EnemyLogicHandler::EnemyHealthFlagRequest, sub, std::placeholders::_1);
    REQUEST_ACTION collisionflagRequest = std::bind(&EnemyLogicHandler::EnemyCollisionFlagRequest, sub, std::placeholders::_1);
    REQUEST_ACTION attackflagRequest = std::bind(&EnemyLogicHandler::EnemyAttackFlagRequest, sub, std::placeholders::_1);
    REQUEST_ACTION pathingflagRequest = std::bind(&EnemyLogicHandler::EnemyPathingFlagRequest, sub, std::placeholders::_1);

    objMessenger.Subscribe(objMessenger, "HealthFlagSet", onHealthFlagChange);
    objMessenger.Subscribe(objMessenger, "CollisionFlagSet", onCollisionFlagChange);
    objMessenger.Subscribe(objMessenger, "AttackFlagSet", onAttackFlagChange);
    objMessenger.Subscribe(objMessenger, "PathingFlagSet", onPathingFlagChange);

    objMessenger.Subscribe(objMessenger, "CollisionStarted", onEnemyCollisionStarted);
    objMessenger.Subscribe(objMessenger, "CollisionEnded", onEnemyCollisionEnded);

    
    objMessenger.SetupRequest("EnemyHealthFlag", healthflagRequest);
    objMessenger.SetupRequest("EnemyCollisionFlag", collisionflagRequest);
    objMessenger.SetupRequest("EnemyAttackFlag", attackflagRequest);
    objMessenger.SetupRequest("EnemyPathingFlag", pathingflagRequest);
  }

  /* Changes the flags present in EnemyLogic Pathing flag. */
  void EnemyLogicHandler::Pathing(EnemyLogic& Enemy)
  {
    GameInstance& parent = Enemy.getParent();

    // Current movement information
    glm::vec2 velocity = parent.RequestData<glm::vec2>("Velocity");
    glm::vec2 acceleration = parent.RequestData<glm::vec2>("Acceleration");
    movement data(velocity, acceleration);

    unsigned EnemyPathingFlag = Enemy.GetEnemyPathingFlag();
    //EnemyPathingFlag = 1;

    //*****************************************//
    /* If there is no pathing, stop everything.*/
    if ( EnemyPathingFlag & ENEMYPATHINGNULL )
    {
      // Set velocity to zero.
      velocity.x = 0;
      velocity.y = 0;
      acceleration.x = 0;
      acceleration.y = 0;
      data.first = velocity;
      data.second = acceleration;
      parent.PostMessage("SetVelocity", Message<glm::vec2>(data.first));
      parent.PostMessage("SetAcceleration", Message<glm::vec2>(data.second));
      return;
    }
    //*****************************************//
    // Has the enemy reached the other side ?
    //*****************************************//
    glm::vec2 pos = parent.RequestData<glm::vec2>("Position");
    Grid& grid = getStage()->GetGrid();
    float scale = getStage()->getInstanceFromID(grid[0][0]).RequestData<glm::vec2>("TileScale").x / 2;
    // xmax = grid width * x scale of tiles
    int xmax = (grid.GetGridWidth() + grid.GetRowOffset()) * scale;
   
    if ( pos.x >= xmax )
    {
      /* Hurt the player! */
      GameInstance * pc = Enemy.getParent().getStage()->getMessenger().Request<GameInstance *>("PlayerController");
      pc->PostMessage("PlayerTakeDamage", Message<int>(1));
      Audio_Engine* AEngine = GetAudioEngine();
      AEngine->PlaySounds("player_hurt.wav", Vector3(), -15.0f);
      Enemy.Die();
    }

    //*****************************************//
    /* Prioritize BUMP over all else. */
    if (EnemyPathingFlag & ENEMYPATHINGBUMP )
    {
      data = PathBump( data );
      if (data.first.x == 0 && data.second.x == 0)
      {
        Enemy.SetEnemyPathingFlag(EnemyPathingFlag & ~ENEMYPATHINGBUMP);
      }

    }
    else
    {
      if (EnemyPathingFlag & ENEMYPATHINGSWITCH)
      {
        data = PathSwitchLane(data);

        /* Removes the switch flag upon completing the switch */
        if (data.first.y == 0 && data.second.y == 0)
        {
          Enemy.SetEnemyPathingFlag(EnemyPathingFlag & ~ENEMYPATHINGSWITCH);
        }
      }

      if (EnemyPathingFlag & ENEMYPATHINGFAST)
      {
        data = PathFast(data);
      }
      if (EnemyPathingFlag & ENEMYPATHINGACCEL)
      {
        data = PathAccel(data);
      }
      if (EnemyPathingFlag & ENEMYPATHINGNORMAL)
      {
        data = PathNormal(data);
      }
    }

    parent.PostMessage("SetVelocity", Message<glm::vec2>(data.first));
    parent.PostMessage("SetAcceleration", Message<glm::vec2>(data.second));
  }

/***********************************************************************************/
  // MESSAGE REQUESTS
/***********************************************************************************/

  void EnemyLogicHandler::EnemyHealthFlagRequest(const EnemyLogic* member, Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = member->GetEnemyHealthFlag();
  }

  void EnemyLogicHandler::EnemyPathingFlagRequest(const EnemyLogic* member, Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = member->GetEnemyHealthFlag();
  }

  void EnemyLogicHandler::EnemyAttackFlagRequest(const EnemyLogic* member, Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = member->GetEnemyAttackFlag();
  }

  void EnemyLogicHandler::EnemyCollisionFlagRequest(const EnemyLogic* member, Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = member->GetEnemyCollisionFlag();
  }

/***********************************************************************************/
/***********************************************************************************/
  EnemyLogic::EnemyLogic( GameInstance* owner ) :
              Component( owner, "EnemyLogic")
  {
  }

  /* Used with .json serialization */
  EnemyLogic::EnemyLogic(GameInstance * owner, const ParsedObject & obj) :
              Component( owner, "EnemyLogic")
  {
    EnemyAttackFlag = obj.getComponentProperty<unsigned>("EnemyLogic", "EnemyAttackFlag");
    EnemyHealthFlag = obj.getComponentProperty<unsigned>("EnemyLogic", "EnemyHealthFlag");
    EnemyCollisionFlag = obj.getComponentProperty<unsigned>("EnemyLogic", "EnemyCollisionFlag");
    EnemyPathingFlag = obj.getComponentProperty<unsigned>("EnemyLogic", "EnemyPathingFlag");
  }

/***********************************************************************************/
  // Enemy Collision functions, also part of messaging.
/***********************************************************************************/
  void EnemyLogic::onCollisionStarted(const Packet& payload )
  {
    /* Do nothing if there is no collision */
    if ( GetEnemyCollisionFlag() & ENEMYCOLLIDENULL )
    {
      return;
    }
    const unsigned othercol = dynamic_cast<const Message<unsigned long> &>(payload).data;

    try
    {
      const GameInstance& other = getParent().getStage()->getInstanceFromID(othercol);

      /* Don't let enemies hit each other. */
      if (dynamic_cast<EnemyLogic*>(other.getComponent("EnemyLogic")))
      {
        return;
      }

      GameInstance& us = getParent();
      int OurHp = us.RequestData<int>("HP");

      const ObjectStats* objother = dynamic_cast<ObjectStats*>(other.getComponent("ObjectStats"));
      // Do nothing if it has no stats.
      if (!objother)
      {
        return;
      }

      unsigned OtherStats = objother->GetStatFlag();

      /* Combat portion of collision */
      // THIS IS ENEMIES TAKING DAMAGE
      if (OtherStats & DAMAGE)
      {
        int dmg = objother->GetDamage();
        OurHp -= dmg;
        Audio_Engine* AEngine = GetAudioEngine();
        AEngine->PlaySounds("attack.wav", Vector3(), -10.0f);
        getParent().PostMessage("HpSet", Message<int>(OurHp));
      }

      /* Does it have StructureLogic? */
      StructureLogic* otherstruct = dynamic_cast<StructureLogic*>(other.getComponent("StructureLogic"));
      if (otherstruct)
      {
        /* Is it able to collide? */
        if (!(otherstruct->GetStructureCollisionFlag() & STRUCTURECOLLIDENULL))
        {
          /* Update collision flag */
          unsigned newflagcol = GetEnemyCollisionFlag() | ENEMYCOLLIDETOWER;
          SetEnemyCollisionFlag(newflagcol);
          unsigned newflagpath = GetEnemyPathingFlag() | ENEMYPATHINGBUMP;
          SetEnemyPathingFlag(newflagpath);
          getParent().PostMessage("CreateParticles", Message<std::string>("explosion"));
        }
      }

      CollisionInteraction(us, other);

      if (OurHp <= 0)
      {
        Die();
      }
    }
    catch (std::out_of_range &)
    {}
  }

  void EnemyLogic::onCollisionEnded(const Packet& payload)
  {
        /* Do nothing if there is no collision */
    if ( GetEnemyCollisionFlag() & ENEMYCOLLIDENULL )
    {
      return;
    }
    const unsigned othercol = dynamic_cast<const Message<unsigned long> &>(payload).data;

    try
    {
      const GameInstance& other = getParent().getStage()->getInstanceFromID(othercol);

      const StructureLogic* otherstruct = dynamic_cast<StructureLogic*>(other.getComponent("StructureLogic"));
      /* Turn pathing back on. */
      if (otherstruct)
      {
        unsigned curr_col = GetEnemyCollisionFlag();
        curr_col &= ~ENEMYCOLLIDETOWER;
        SetEnemyCollisionFlag(curr_col);
      }

    }
    catch(const std::out_of_range &)
    {}
  }

/*******************************************************************************************/
  // Message system operations -- takes packet info and connects functions (Ex: to perform combat. )
/*******************************************************************************************/
  void EnemyLogic::OnEnemyHealthFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetEnemyHealthFlag( newstats );
  }

  void EnemyLogic::OnEnemyCollisionFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetEnemyCollisionFlag( newstats );
  }

  void EnemyLogic::OnEnemyAttackFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetEnemyAttackFlag( newstats );
  }

  void EnemyLogic::OnEnemyPathingFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetEnemyPathingFlag( newstats );
  }
/*******************************************************************************************/
  // Getters for data in the class.
/*******************************************************************************************/
  unsigned EnemyLogic::GetEnemyAttackFlag() const
  {
    return EnemyAttackFlag;
  }

  unsigned EnemyLogic::GetEnemyCollisionFlag() const
  {
    return EnemyCollisionFlag;
  }

  unsigned EnemyLogic::GetEnemyHealthFlag() const
  {
    return EnemyHealthFlag;
  }

  unsigned EnemyLogic::GetEnemyPathingFlag() const
  {
    return EnemyPathingFlag;
  }

/*******************************************************************************************/
  // Setters for data in the class.
/*******************************************************************************************/
  void EnemyLogic::SetEnemyAttackFlag( unsigned val )
  {
    EnemyAttackFlag = val;
  }

  void EnemyLogic::SetEnemyCollisionFlag( unsigned val )
  {
    EnemyCollisionFlag = val;
  }

  void EnemyLogic::SetEnemyHealthFlag( unsigned val )
  {
    EnemyHealthFlag = val;
  }

  void EnemyLogic::SetEnemyPathingFlag( unsigned val )
  {
    EnemyPathingFlag = val;
  }

  // Sets all flags to NULL so the object still exists but does nothing.
  //    A polite way of saying, "you're dead to me"
  void EnemyLogic::Die()
  {
    unsigned atkflg = GetEnemyAttackFlag() | ENEMYATTACKNULL;
    unsigned colflg = GetEnemyCollisionFlag() | ENEMYCOLLIDENULL;
    unsigned hpflg = GetEnemyHealthFlag() | ENEMYHEALTHNULL;
    unsigned pathflg = GetEnemyPathingFlag() | ENEMYPATHINGNULL;
    SetEnemyAttackFlag( atkflg );
    SetEnemyCollisionFlag( colflg );
    SetEnemyHealthFlag( hpflg );
    SetEnemyPathingFlag( pathflg );

    //GameInstance& parent = getParent();

    //GRAPHICAL_ELEMENT_PTR wowlookatthatgraphic = parent.RequestData<GRAPHICAL_ELEMENT_PTR>("Graphic");
   // wowlookatthatgraphic->SetVisible(false);
    getParent().PostMessage("CreateParticles", Message<std::string>("explosion"));
    Stage* condemnation = getParent().getStage();
    condemnation->removeGameInstance(getParent().getId());
  }

  void EnemyLogic::Live()
  {
    unsigned atkflg = GetEnemyAttackFlag() & ~ENEMYATTACKNULL;
    unsigned colflg = GetEnemyCollisionFlag() & ~ENEMYCOLLIDENULL;
    unsigned hpflg = GetEnemyHealthFlag() & ~ENEMYHEALTHNULL;
    unsigned pathflg = GetEnemyPathingFlag() & ~ENEMYPATHINGNULL;
    SetEnemyAttackFlag( atkflg );
    SetEnemyCollisionFlag( colflg );
    SetEnemyHealthFlag( hpflg );
    SetEnemyPathingFlag( pathflg );

    GameInstance& parent = getParent();

    DrawToken wowlookatthatgraphic = parent.RequestData<DrawToken>("Graphic");
    wowlookatthatgraphic.setVisible(true);



  }

  /* SPAWN ENEMY */
  // The data types exist to help show what is needed to spawn an enemy.
  // enemydata is what belongs to the enemy structure.
  // objectdata is general information that most game objects share
  unsigned long SpawnEnemy(Stage& stage, ObjectSpawnInfo& objectdata, EnemySpawnFlags& enemydata, glm::vec2 pos, bool null )
  {
    if ( enemydata.size() < 4 )
    {
      /* Throw an exception, too little info */
      //throw;
    }
    GameInstance& enemy = stage.addGameInstance("Enemy1");

    enemy.PostMessage("StatFlagSet", Message<unsigned>(objectdata.first));
    enemy.PostMessage("HpSet", Message<int>(objectdata.second.first));
    enemy.PostMessage("DamageSet", Message<int>(objectdata.second.second));
    enemy.PostMessage("HealthFlagSet", Message<unsigned>(enemydata[0]));
    enemy.PostMessage("CollisionFlagSet", Message<unsigned>(enemydata[1]));
    enemy.PostMessage("AttackFlagSet", Message<unsigned>(enemydata[2]));
    enemy.PostMessage("PathingFlagSet", Message<unsigned>(enemydata[3]));

    /* Set the colors of the enemies */
    DrawToken wowlookatthatgraphic = enemy.RequestData<DrawToken>("Graphic");
    if ( enemydata[3] == ENEMYPATHINGNORMAL )
    {
      GetAudioEngine()->PlaySounds("EnemySpawn.wav", Vector3(), 0.5f);
    }
    if ( enemydata[3] == ENEMYPATHINGFAST)
    {
      wowlookatthatgraphic.setShade( glm::vec4(0.2f, 0.9f, 0.9f, 1) );
      GetAudioEngine()->PlaySounds("EnemySpawn2.wav", Vector3(), -3.0f);
    }
    if ( enemydata[3] == ENEMYPATHINGACCEL)
    {
      wowlookatthatgraphic.setShade(glm::vec4{ 0.85f, 0.2f, 0.2f, 1});
      GetAudioEngine()->PlaySounds("EnemySpawn3.wav", Vector3(), -10.0f);
    }

    glm::vec2 mov;
    mov.x = 0;
    mov.y = 0;

    enemy.PostMessage("SetVelocity", Message<glm::vec2>(mov));
    enemy.PostMessage("SetAcceleration", Message<glm::vec2>(mov));

    enemy.PostMessage("PositionSet", Message<glm::vec2>(pos));
    if ( null )
    {
      dynamic_cast<EnemyLogic*>(enemy.getComponent("EnemyLogic"))->Die();
    }

    return enemy.getId();
  }

  /* Gets the location for spawning an enemy in a random or specified location on one side of the grid */
  glm::vec2 ReturnEnemySpawnLocation( Grid* grid, int col, int size )
  {
    int height = grid->GetGridHeight();
    int width = grid->GetGridWidth();
    int placement_width = -width; /* Start on the left side. */
    
    /* Don't return locations outside of the grid */
    if ( col )
    {
      if (col <= 0 || col > height )
      {
        col = Utils::random_uniform(0, height + 1);
      }
    }
    //float x = -(height) * 37.5;
    //float y = (placement_width) * 37.5 + col * 75 - 37.5;
    Stage* stage = grid->GetStage();
    glm::vec2 pos = stage->getInstanceFromID((*grid)[(col - 1) % height][0]).RequestData<glm::vec2>("Position");
    return pos;
  }

  void ReturnEnemyArchetype(ObjectSpawnInfo* objectdata, EnemySpawnFlags* enemydata, std::string Enemy)
  {
    if ( enemydata->size() != 0 )
    {
      enemydata->clear();
    }
    /* Has 20 HP, 5 dmg */
    if ( Enemy == "Normal" )
    {
      objectdata->first = 3;
      objectdata->second.first = 5;
      objectdata->second.second = 5;
      (*enemydata).push_back(ENEMYHEALTHNORMAL);
      (*enemydata).push_back(0);
      (*enemydata).push_back(ENEMYATTACKMELEE);
      (*enemydata).push_back(ENEMYPATHINGNORMAL);

    }
    else if ( Enemy == "Fast" )
    {
      objectdata->first = 3;
      objectdata->second.first = 10;
      objectdata->second.second = 5;
      (*enemydata).push_back(ENEMYHEALTHNORMAL);
      (*enemydata).push_back(0);
      (*enemydata).push_back(ENEMYATTACKMELEE);
      (*enemydata).push_back(ENEMYPATHINGFAST);
    }
    else if (Enemy == "Accel")
    {
      objectdata->first = 3;
      objectdata->second.first = 15;
      objectdata->second.second = 10;
      (*enemydata).push_back(ENEMYHEALTHNORMAL);
      (*enemydata).push_back(0);
      (*enemydata).push_back(ENEMYATTACKMELEE);
      (*enemydata).push_back(ENEMYPATHINGACCEL);
    }
    else
    {
      Log<Error>("Invalid enemy type '%s' given", Enemy.c_str());
      throw std::runtime_error("Invalid enemy type given");
    }
  }

} // namespace Engine
