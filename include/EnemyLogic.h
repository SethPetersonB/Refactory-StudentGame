// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "../include/GameInstance.h"
#include "../include/object_stats.h"
#include "../include/Physics.h"
#include "../include/grid.h"


namespace Engine
{
  typedef std::vector<std::pair<ObjectStats*, ObjectStats*>> TargetVec;
  typedef std::pair<int, ObjectStats*> EnemyMessageData;
  typedef std::vector<unsigned> EnemySpawnFlags; // Always give only 4 entries.

  class EnemyLogicHandler;
  class EnemyLogic;
  /* Not all of these fwd declarations are necessary... */
  class Physics;
  class PhysicsHandler;
  class ColliderHandler;
  class Collider;
  class Collision;
  class GameInstance;
  class CircleCollider;
  class RectangleCollider;
  

  class EnemyLogicHandler : public ComponentHandler
  {
  public:
    EnemyLogicHandler( Stage* stage );

    virtual ~EnemyLogicHandler() {};
    void update();
    void ConnectEvents( Component * base_sub );
    void Pathing( EnemyLogic& Enemy );

    static void EnemyHealthFlagRequest( const EnemyLogic* member, Packet& data );
    static void EnemyPathingFlagRequest( const EnemyLogic* member, Packet& data );
    static void EnemyAttackFlagRequest( const EnemyLogic* member, Packet& data );
    static void EnemyCollisionFlagRequest( const EnemyLogic* member, Packet& data );
  };

  class EnemyLogic : public Component
  {    
  public:
    EnemyLogic( GameInstance* owner );
    EnemyLogic(GameInstance * owner, const ParsedObject & obj);

    void onCollisionStarted(const Packet& payload );
    void onCollisionEnded(const Packet& payload );
    void OnEnemyAttackFlagChange( const Packet& payload );
    void OnEnemyCollisionFlagChange( const Packet& payload );
    void OnEnemyHealthFlagChange( const Packet& payload );
    void OnEnemyPathingFlagChange( const Packet& payload );
    void Die();
    void Live();
    unsigned GetEnemyAttackFlag() const;
    unsigned GetEnemyCollisionFlag() const;
    unsigned GetEnemyHealthFlag() const;
    unsigned GetEnemyPathingFlag() const;
    void SetEnemyAttackFlag( unsigned val );
    void SetEnemyCollisionFlag( unsigned val );
    void SetEnemyHealthFlag( unsigned val );
    void SetEnemyPathingFlag( unsigned val );
  private:
    unsigned EnemyAttackFlag;
    unsigned EnemyCollisionFlag;
    unsigned EnemyHealthFlag;
    unsigned EnemyPathingFlag;
  };

  glm::vec2 ReturnEnemySpawnLocation(Grid* grid, int col = 1, int size = 1);
  unsigned long SpawnEnemy( Stage& stage, ObjectSpawnInfo& objectdata, EnemySpawnFlags& enemydata, glm::vec2 pos, bool null = 0 );
  void ReturnEnemyArchetype( ObjectSpawnInfo* objectdata, EnemySpawnFlags* enemydata, std::string Enemy );

  enum ENEMYATTACKFILTER
  {
    ENEMYATTACKMELEE = 1 << 0, // Deal damage on collision.
    ENEMYATTACKNULL = 1 << 1, // Attacking is skipped.
  };

  enum ENEMYCOLLISIONFILTER
  {
    ENEMYCOLLIDENULL = 1 << 0,   // Collision is skipped.
    ENEMYCOLLIDEBULLET = 1 << 1, // Deal damage on collision.
    ENEMYCOLLIDEFINISH = 1 << 2, // Enemy reaches the end, dies.
    ENEMYCOLLIDETOWER = 1 << 3, // Stop when an enemy hits a tower.
  };

  enum ENEMYPATHINGFILTER
  {
    ENEMYPATHINGNULL = 1 << 0, // Pathing is skipped.
    ENEMYPATHINGNORMAL = 1 << 1, // Walk in a straight line.
    ENEMYPATHINGSWITCH = 1 << 2, // Switches lanes when Health drops low ( 1/3 ?)
    ENEMYPATHINGFAST = 1 << 3, // Moves faster than normal.
    ENEMYPATHINGACCEL = 1 << 4, // Gradually gains speed.
    ENEMYPATHINGBUMP = 1 << 5, // "Switches" lanes backwards to help w/collision.
  };

  enum ENEMYHEALTHFILTER
  {
    ENEMYHEALTHNULL = 1 << 0, // Health is skipped.
    ENEMYHEALTHNORMAL = 1 << 1, // Dies when Health drops below 0
  };
}