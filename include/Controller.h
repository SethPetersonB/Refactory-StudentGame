// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "Input.h"
#include "Timer.h"
//#include "structures.h"
#include "Structure.h"

namespace Engine
{
#define MAX_STRUCTURE_HEIGHT 2
#define REGEN_RATE 2

  class Controller : public Component
  {
  public:
    Controller(GameInstance* owner);
    Controller(GameInstance* owner, const ParsedObject & obj);
    virtual ~Controller() {};

    void autoPlay();
    void Reset();

    bool IsWavesRunning() const { return wavesRunning_; }
    void UpdateInput();
    int getHP() const;
    int getWallsAvailable() const;
    bool getAuto() const;
    Timer& getRegenTimer();
    std::vector<glm::vec3>& getPosition();
    std::vector<glm::vec3>& getID();
    STRUCT_LIST * getParsedStructs();
    OBJECT_BY_TILE_LIST* getBlocksAtTileList();

    void toggleAuto();
    void setAuto(bool state) { autoplay_ = state; }
    void setGod(bool state);

    void setHP(int hp);
    void setWallsAvailable(int walls);
    void takeDamage(int dmg);
    void placeWall();
    void UpdateCount();
    void setWaveNumber(size_t num);
    void setWaveCount(size_t num);
    
    unsigned long GetHPCountID() const { return healthId_; }
    unsigned long GetResourceCountID() const { return ammoId_; }
    unsigned long GetProgressID() const { return progressId_; }
    unsigned long GetBeginID() const { return beginId_; }

    void SetResourceID(unsigned long id) { ammoId_ = id; }
    void SetHealthID(unsigned long id) { healthId_ = id; }
    void SetProgressID(unsigned long id) { progressId_ = id; }
    void SetWaveTextID(unsigned long id) { waveTextId_ = id; }
    void SetWaveCountID(unsigned long id) { waveId_ = id; }
    void SetBeginID(unsigned long id) { beginId_ = id; }
    void SetWavesRunning(bool run) { wavesRunning_ = run; }
    void ToggleGodMode();
    bool IsGod() const;

    // messenging functions
    void onSetHP(const Packet& payload);
    void onSetWalls(const Packet& payload);
    void onTakeDamage(const Packet& payload);
    void onPlaceWall(const Packet& payload);
    void onToggleAuto(const Packet& payload);
    void autoPlayEvent(const Packet& payload);
    void LoseEvent(const Packet& payload);
    void onWaveChanged(const Packet & payload);
    void onSetHPFrame(const Packet & payload);
    void onSetResourceFrame(const Packet & paylaod);
    void onStagePaused(const Packet & payload);

    bool isLost();
    bool isWon();
    void setLost(bool result);
    void setWon(bool result);

    std::vector<glm::vec2> & getTilePositions() { return tile_positions_; }

    void HPRequest(Packet& data);
    void WallRequest(Packet& data);
    void ownerRequest(Packet& data);
    void autoRequest(Packet& data);
    void positionsRequest(Packet& data);
    void IDRequest(Packet& data);
    void LSLRequest(Packet& data);
    void tilePositionRequest(Packet& data);
    void tileBlocksRequest(Packet& data);

    void OnWavePaused(const Packet & payload);
    void Begin();

  private:
    bool won_ = false;
    bool lost_ = false;

    unsigned health_;
    unsigned walls_available_;
    bool godMode_;
    bool autoplay_;
    bool wavesRunning_;

    unsigned long healthId_;    // Instance ID of the ammo counter
    unsigned long ammoId_;      // Instance ID of the health counter
    unsigned long progressId_;  // Instance ID of wave progress
    unsigned long waveId_;      // Instance ID of wave number counter
    unsigned long waveTextId_;  // Instance ID of wave text
    unsigned long beginId_;

    unsigned waveNum_;
    unsigned waveCount_;

    Timer auto_timer_; // autoplay timer
    Timer regen_timer_; // block regen timer

    std::vector<glm::vec3> positions_;
    std::vector<glm::vec3> ID_;
    std::vector<glm::vec2> tile_positions_;

    OBJECT_BY_TILE_LIST BlocksAtTileList;

    STRUCT_LIST LoadedStructureList;
  };

  class ControllerHandler : public ComponentHandler
  {
  public:
    ControllerHandler(Stage* stage);
    virtual ~ControllerHandler() {};

    void update();

    void getLuaRegisters() override;


  protected:
    void ConnectEvents(Component * base_sub);

  };
}
