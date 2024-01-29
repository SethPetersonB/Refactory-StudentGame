// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/object_stats.h"
#include "Stage.h"

namespace Engine
{
  ObjectStatsHandler::ObjectStatsHandler(Stage* stage) : ComponentHandler( stage, "ObjectStats" )
  {
  }

  static void MaxHPRequest(const ObjectStats * obj, Packet & data)
  {
    data.setData(obj->GetMaxHP());
  }

  void ObjectStatsHandler::ConnectEvents(Component * base_sub)
  {
    ObjectStats * sub = dynamic_cast<ObjectStats *>(base_sub);

    using namespace std;

    Messenger & objMessenger = sub->getParent().getMessenger();

    SUBSCRIBER_ACTION onStatFlagChange = std::bind(&ObjectStats::OnStatFlagChange, sub, placeholders::_1);

    REQUEST_ACTION hpRequest = std::bind(&ObjectStatsHandler::HpRequest, sub, placeholders::_1);
    REQUEST_ACTION maxHPReq = std::bind(MaxHPRequest, sub, placeholders::_1);
    REQUEST_ACTION dmgRequest = std::bind(&ObjectStatsHandler::DmgRequest, sub, placeholders::_1);
    REQUEST_ACTION statRequest = std::bind(&ObjectStatsHandler::StatFlagRequest, sub, placeholders::_1);

    objMessenger.Subscribe(objMessenger, "StatFlagSet", onStatFlagChange);

    objMessenger.SetupRequest("HP", hpRequest);
    objMessenger.SetupRequest("MaxHP", maxHPReq);
    objMessenger.SetupRequest("DMG", dmgRequest);
    objMessenger.SetupRequest("STATS", statRequest);

    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<int>(objMessenger, "HpSet"));
    events.push_back(router.newEvent<int>(objMessenger, "DamageSet"));
    sub->getParent().registerScriptEvent(events);
  }

  void ObjectStats::OnStatFlagChange(const Packet& payload)
  {
    unsigned newstats = dynamic_cast<const Message<unsigned> *>(&payload)->data;
    SetStatFlag( newstats );
  }

  void ObjectStatsHandler::HpRequest(const ObjectStats* member, Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = member->GetHitPoints();
  }

  void ObjectStatsHandler::DmgRequest(const ObjectStats* member, Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = member->GetDamage();
  }

  void ObjectStatsHandler::StatFlagRequest(const ObjectStats* member, Packet& data)
  {
    dynamic_cast<Message<unsigned> &>(data).data = member->GetStatFlag();
  }

  void ObjectStatsHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<ObjectStats, Component>("ObjectStats")
      .property("hp", &ObjectStats::GetHitPoints, &ObjectStats::SetHitPoints)
      .property("maxHp", &ObjectStats::GetHitPoints, &ObjectStats::SetMaxHitPoints)
      .property("damage", &ObjectStats::GetDamage, &ObjectStats::SetDamage)
      
    );
  }

  void ObjectStatsHandler::update()
  {
    for (auto & component : componentList_)
    {
      ObjectStats* comp = static_cast<ObjectStats*>(component);

      if (comp->GetHitPoints() <= 0)
        stage_->removeGameInstance(comp->getParent());
    }
  }


  ObjectStats::ObjectStats(GameInstance* owner, unsigned StatFlag_, int HitPoints_, int Damage_) : Component( owner, "ObjectStats" ),
     StatFlag(StatFlag_), HitPoints(HitPoints_), Damage(Damage_) 
  {
  }

  ObjectStats::ObjectStats(GameInstance * owner, const ParsedObject & obj) : Component( owner, "ObjectStats" )
  {
    StatFlag = obj.getComponentProperty<unsigned>("ObjectStats", "StatFlag");
    maxHP = obj.getComponentProperty<int>("ObjectStats", "HitPoints");
    HitPoints = maxHP;
    Damage = obj.getComponentProperty<int>("ObjectStats", "Damage");
  }


  unsigned ObjectStats::GetStatFlag() const
  {
    return StatFlag;
  }

  void ObjectStats::SetStatFlag(unsigned flag)
  {
    StatFlag = flag;
  }

  int ObjectStats::GetHitPoints() const
  {
    return HitPoints;
  }

  void ObjectStats::SetHitPoints(int val)
  {
    HitPoints = (val <= GetMaxHP()) ? val : GetMaxHP();
  }

  void ObjectStats::SetMaxHitPoints(int val)
  {
    // Using getters since HP/Max may change with damage calculations(?) in the future
    int currHp = GetMaxHP();  
    int currMax = GetMaxHP();

    maxHP = val;

    // Preserves max HP if at max HP before change, and makes sure current HP doesn't overflow
    SetHitPoints((currHp == currMax) ? GetMaxHP() : currHp);
  }

  int ObjectStats::GetDamage() const
  {
    return Damage;
  }

  void ObjectStats::SetDamage(int val)
  {
    Damage = val;
  }

}