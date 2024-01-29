// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once


#include "../include/GameInstance.h"

namespace Engine
{
  typedef std::pair<unsigned,std::pair<int,int>> ObjectSpawnInfo;
  
  class ObjectStats;

  class ObjectStatsHandler : public ComponentHandler
  {
  public:
    ObjectStatsHandler( Stage* stage );
    virtual ~ObjectStatsHandler() {};
    void update();
    void ConnectEvents(Component * base_sub);
    static void DmgRequest(const ObjectStats* member, Packet& data);
    static void HpRequest( const ObjectStats* member, Packet& data );
    static void StatFlagRequest( const ObjectStats* member, Packet& data);

    void getLuaRegisters() override;
  private:
  };

  class ObjectStats : public Component
  {
  public:
    ObjectStats(GameInstance* owner, unsigned StatFlag_ = 0, int HitPoints_ = 0, int Damage_ = 0 );
    ObjectStats(GameInstance * owner, const ParsedObject & obj);

    unsigned GetStatFlag() const;
    int GetHitPoints() const;
    int GetMaxHP() const { return maxHP; }
    int GetDamage() const;
    void OnStatFlagChange(const Packet& payload);
    void SetStatFlag(unsigned flag);
    void SetHitPoints(int val);
    void SetMaxHitPoints(int val);
    void SetDamage(int val);
  private:
    unsigned StatFlag = 0;
    int      HitPoints = 0;
    int maxHP = 0;
    int      Damage = 0;
  };

  enum STATFILTER
  {
    HITPOINTS = 1 << 0,
    DAMAGE = 1 << 1,
  };
}