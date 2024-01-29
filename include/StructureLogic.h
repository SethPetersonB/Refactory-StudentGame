// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "../include/GameInstance.h"
#include "../include/object_stats.h"
#include "../include/Physics.h"

namespace Engine
{
  class StructureLogic;
  /* Not all of these fwd declarations are necessary... */
  class Physics;
  class PhysicsHandler;
  class ColliderHandler;
  class Collider;
  class Collision;
  class GameInstance;
  class CircleCollider;
  class RectangleCollider;
  

  class StructureLogicHandler : public ComponentHandler
  {
  public:
    StructureLogicHandler( Stage* stage );

    virtual ~StructureLogicHandler() {};
    void update();
    void ConnectEvents( Component * base_sub );
    void getLuaRegisters() override;

    static void StructureHealthFlagRequest( const StructureLogic* member, Packet& data );
    static void StructureAttackFlagRequest( const StructureLogic* member, Packet& data );
    static void StructureCollisionFlagRequest( const StructureLogic* member, Packet& data );
  };

  class StructureLogic : public Component
  {    
  public:
    StructureLogic( GameInstance* owner );
    StructureLogic(GameInstance * owner, const ParsedObject & obj);

    void onCollisionStarted(const Packet& payload );
    void OnStructureAttackFlagChange( const Packet& payload );
    void OnStructureCollisionFlagChange( const Packet& payload );
    void OnStructureHealthFlagChange( const Packet& payload );
    void Die();
    void Live();
    unsigned GetStructureAttackFlag() const;
    unsigned GetStructureCollisionFlag() const;
    unsigned GetStructureHealthFlag() const;
    void SetStructureAttackFlag( unsigned val );
    void SetStructureCollisionFlag( unsigned val );
    void SetStructureHealthFlag( unsigned val );

    unsigned GetShield() const { return shield_; }
    void     SetShield(unsigned shield) { shield_ = shield; }

    unsigned GetImmuneTurn() const { return immuneTurn_; }
    void     SetImmuneTurn(unsigned turn) { immuneTurn_ = turn; }

    unsigned GetTimesHit() const { return timesHit_; }
    void     SetTimesHit(unsigned hit) { timesHit_ = hit; }
  private:
    unsigned StructureAttackFlag;
    unsigned StructureCollisionFlag;
    unsigned StructureHealthFlag;
    unsigned shield_ = 0; // shield power for shield structures
    unsigned immuneTurn_ = 0;
    unsigned timesHit_ = 0;
    /* Possible list of parts of grid that constitute the building */
  };

  enum STRUCTUREATTACKFILTER
  {
    STRUCTUREATTACKMELEE = 1 << 0, // Deal damage on collision.
    STRUCTUREATTACKNULL = 1 << 1, // Attacking is skipped.
  };

  enum STRUCTURECOLLISIONFILTER
  {
    STRUCTURECOLLIDENULL = 1 << 0,   // Collision is skipped.
    STRUCTURECOLLIDEBULLET = 1 << 1, // Deal damage on collision.
    STRUCTURECOLLIDENEMY = 1 << 2, // Enemy reaches the end, dies.
  };

  enum STRUCTUREHEALTHFILTER
  {
    STRUCTUREHEALTHNULL = 1 << 0, // Health is skipped.
    STRUCTUREHEALTHNORMAL = 1 << 1, // Dies when Health drops below 0
  };
}